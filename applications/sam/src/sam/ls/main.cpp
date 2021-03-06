#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Detector.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace sp  = sequenceParser;

namespace sam
{
	Color _color;
	bool wasSthgDumped = false;
}

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
	return os;
}

template<class T>
void coutVec( const boost::ptr_vector<T>& v )
{
	BOOST_FOREACH( const T& f, v )
	{
		std::cout << f << std::endl;
		sam::wasSthgDumped = true;
	}
}


int main( int argc, char** argv )
{
	using namespace tuttle::common;
	using namespace sam;

	sp::EMaskType                 researchMask        = sp::eMaskTypeDirectory | sp::eMaskTypeFile | sp::eMaskTypeSequence ; // by default show directories, files and sequences
	sp::EMaskOptions              descriptionMask     = sp::eMaskOptionsNone;   // by default show nothing
	bool                                      recursiveListing    = false;
	bool                                      script              = false;
	bool                                      enableColor         = false;
	std::string                               availableExtensions;
	std::vector<std::string>                  paths;
	std::vector<std::string>                  filters;
	sp::Detector                  detector;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		(kAllOptionString            , kAllOptionMessage)
		(kDirectoriesOptionString    , kDirectoriesOptionMessage)
		(kExpressionOptionString     , bpo::value<std::string>(), kExpressionOptionMessage)
		(kFilesOptionString          , kFilesOptionMessage )
		(kHelpOptionString           , kHelpOptionMessage)
		(kLongListingOptionString   , kLongListingOptionMessage)
		(kRelativePathOptionString  , kRelativePathOptionMessage)
		(kRecursiveOptionString      , kRecursiveOptionMessage)
		(kPathOptionString    , kPathOptionMessage)
		(kSequencesOptionString           , kSequencesOptionMessage)
		(kColorOptionString            , kColorOptionMessage)
		(kFullDisplayOptionString     , kFullDisplayOptionMessage )
		(kScriptOptionString           , kScriptOptionMessage)
		(kBriefOptionString            , kBriefOptionMessage)
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		(kInputDirOptionString, bpo::value< std::vector<std::string> >(), kInputDirOptionMessage)
		(kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage)
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add(kInputDirOptionLongName, -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);
	
	bpo::variables_map vm;

	try
	{
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

		// get environment options and parse them
		if( const char* env_ls_options = std::getenv("SAM_LS_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_ls_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		if( const char* env_ls_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_ls_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_COUT("sam-ls: command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_COUT("sam-ls: unknown error in command line.");
		exit( -2 );
	}

	if ( vm.count(kScriptOptionLongName) )
	{
		// disable color, disable directory printing and set relative path by default
		script = true;
		descriptionMask |= sp::eMaskOptionsAbsolutePath;
	}

	if ( vm.count(kColorOptionLongName) && !script )
	{
		enableColor = true;
	}
	if ( vm.count(kEnableColorOptionLongName) && !script )
	{
		const std::string str = vm[kEnableColorOptionLongName].as<std::string>();
		enableColor = string_to_boolean( str );
	}

	if( enableColor )
	{
		descriptionMask |= sp::eMaskOptionsColor;
		_color.enable();
	}

	if (vm.count(kHelpOptionLongName))
	{
		TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-ls - list directory contents" << _color._std << std::endl);
		TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-ls [options] [directories]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std << std::endl );

		TUTTLE_COUT( "List information about the sequences, files and folders." );
		TUTTLE_COUT( "List the current directory by default, and only sequences." );
		TUTTLE_COUT( "The script option disable color, disable directory printing (in multi-directory case or recursive) and set relative path by default." << std::endl );

		TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std << std::endl );
		TUTTLE_COUT( mainOptions );
		return 0;
	}

	if ( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_COUT( _color._green << "list directory contents" << _color._std);
		return 0;
	}

	if (vm.count(kExpressionOptionLongName))
	{
		TUTTLE_COUT( _color._red << "Expression: " << vm["expression"].as<std::string>() << _color._std );
		bal::split( filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}

	if( vm.count(kDirectoriesOptionLongName ) | vm.count(kFilesOptionLongName) | vm.count(kSequencesOptionLongName) )
	{
		researchMask &= ~sp::eMaskTypeDirectory;
		researchMask &= ~sp::eMaskTypeFile;
		researchMask &= ~sp::eMaskTypeSequence;
	}
		
	if( vm.count(kDirectoriesOptionLongName ) )
	{
		researchMask |= sp::eMaskTypeDirectory;
	}
	if (vm.count(kFilesOptionLongName))
	{
		researchMask |= sp::eMaskTypeFile;
	}
	if (vm.count(kSequencesOptionLongName))
	{
		researchMask |= sp::eMaskTypeSequence;
	}
	
	if (vm.count(kFullDisplayOptionLongName))
	{
		researchMask |= sp::eMaskTypeDirectory;
		researchMask |= sp::eMaskTypeFile;
		researchMask |= sp::eMaskTypeSequence;
	}
	
	if (vm.count(kAllOptionLongName))
	{
		// add .* files
		descriptionMask |= sp::eMaskOptionsDotFile;
	}
	
	if (vm.count(kLongListingOptionLongName))
	{
		descriptionMask |= sp::eMaskOptionsProperties;
	}
	
	if (vm.count(kRelativePathOptionLongName) )
	{
		descriptionMask |= sp::eMaskOptionsPath;
	}

	if(vm.count(kPathOptionLongName))
	{
		descriptionMask |= sp::eMaskOptionsAbsolutePath;
	}
	
	// defines paths, but if no directory specify in command line, we add the current path
	if (vm.count(kInputDirOptionLongName))
	{
		paths = vm[kInputDirOptionLongName].as< std::vector<std::string> >();
	}
	else
	{
		paths.push_back( "./" );
	}
	
	if (vm.count(kRecursiveOptionLongName))
	{
		recursiveListing = true;
	}


// 	for(uint i=0; i<filters.size(); i++)
// 	  TUTTLE_COUT("filters = " << filters.at(i));
// 	TUTTLE_COUT("research mask = " << researchMask);
// 	TUTTLE_COUT("options  mask = " << descriptionMask);

	std::list<boost::shared_ptr<sp::FileObject> > listing;
	try
	{
		std::size_t index = 0;
		BOOST_FOREACH( bfs::path path, paths )
		{
			if( path == bfs::path(".") )
			{
				path = bfs::path("./");
			}
			if( ( paths.size() > 1 || recursiveListing ) && !script )
			{
				if( index > 0 )
				{
					TUTTLE_COUT( "" );
				}
				TUTTLE_COUT( path.string() << ":");
				wasSthgDumped = true;
			}

			coutVec( detector.fileObjectInDirectory( path.string(), filters, researchMask, descriptionMask ) );

			if(recursiveListing)
			{
				for ( bfs::recursive_directory_iterator end, dir( path ); dir != end; ++dir )
				{
					if( bfs::is_directory( *dir ) )
					{
						bfs::path currentPath = (bfs::path)*dir;
						if( !script )
							TUTTLE_COUT( "\n" << currentPath.string() << ":" );

						coutVec( detector.fileObjectInDirectory( currentPath.string(), filters, researchMask, descriptionMask ) );

					}
				}
			}
			++index;
		}
	}
	catch ( const bfs::filesystem_error& ex)
	{
		TUTTLE_COUT( ex.what() );
	}
	catch( ... )
	{
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}
	if(!wasSthgDumped)
	        TUTTLE_CERR ( _color._error << "No sequence found here." << _color._std );
	return 0;
}
