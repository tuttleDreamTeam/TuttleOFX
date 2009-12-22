#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>



//TODO: delete these include
#include <tuttle/host/core/Param.hpp>
#include <tuttle/host/core/ClipImage.hpp>


namespace tuttle {
namespace host {
namespace core {


ProcessGraph::ProcessGraph( Graph& graph )
: _graph( graph.getGraph() )
, _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	_graph.transpose();
	relink();
}

ProcessGraph::~ProcessGraph()
{
}

void ProcessGraph::relink()
{
	Graph::InternalGraph::vertex_range_t vrange = _graph.getVertices();
	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		v.setProcessNode( &_nodes.at( v.processNode()->getName() ) );
	}
}

void ProcessGraph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	//graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "test.dot" );

	// Initialize variables
	const int numFramesToRender = tEnd - tBegin;
	OfxPointD renderScale = { 1.0, 1.0 };
	OfxRectI renderWindow = { 0, 0,	123, 123 };


	//--- BEGIN RENDER
	ProcessOptions processOptions;
	processOptions._startFrame = tBegin;
	processOptions._endFrame = tEnd;
	processOptions._step = 1;
	processOptions._interactive = false;
	processOptions._renderScale = renderScale;

	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes ){
		p.second->begin(processOptions);
	}

	//--- RENDER
	for( int t = 0; t < numFramesToRender; ++t )
	{
		Graph::InternalGraph optimizedGraph(_graph);

		processOptions._time = t;
		processOptions._field = kOfxImageFieldBoth;
		processOptions._renderRoI = renderWindow;
		processOptions._renderScale = renderScale;

		TCOUT("---------------------------------------- connectClips");
		core::dfs_connectClips_visitor connectClipsVisitor(processOptions);
		optimizedGraph.dfs(connectClipsVisitor);

		TCOUT("---------------------------------------- precompute");
		core::dfs_preCompute_visitor preComputeVisitor(processOptions);
		optimizedGraph.dfs(preComputeVisitor);

		TCOUT("---------------------------------------- compute");
		core::dfs_compute_visitor computeVisitor(processOptions);
		optimizedGraph.dfs(computeVisitor);

		TCOUT("---------------------------------------- postcompute");
		core::dfs_postCompute_visitor postComputeVisitor(processOptions);
		optimizedGraph.dfs(postComputeVisitor);
	}


	//--- END RENDER
	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes ){
		processOptions._startFrame = tBegin;
		processOptions._endFrame = tEnd;
		processOptions._step = 1;
		processOptions._interactive = false;
		processOptions._renderScale = renderScale;
		p.second->end(processOptions);
	}
}

}
}
}
