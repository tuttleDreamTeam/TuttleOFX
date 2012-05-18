/****************************************************************************
** Meta object code from reading C++ file 'main.h'
**
** Created: Sat Mar 10 20:43:53 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PFSViewMainWin[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      37,   15,   15,   15, 0x0a,
      57,   15,   15,   15, 0x0a,
      98,   82,   15,   15, 0x0a,
     123,   15,   15,   15, 0x0a,
     141,   15,   15,   15, 0x0a,
     157,   15,   15,   15, 0x0a,
     177,   15,   15,   15, 0x0a,
     189,   15,   15,   15, 0x0a,
     208,  201,   15,   15, 0x0a,
     240,  233,   15,   15, 0x0a,
     270,  233,   15,   15, 0x0a,
     294,   15,   15,   15, 0x0a,
     312,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PFSViewMainWin[] = {
    "PFSViewMainWin\0\0updatePointerValue()\0"
    "updateRangeWindow()\0updateChannelSelection()\0"
    "selectedChannel\0setChannelSelection(int)\0"
    "updateZoomValue()\0gotoNextFrame()\0"
    "gotoPreviousFrame()\0saveImage()\0"
    "copyImage()\0method\0setLumMappingMethod(int)\0"
    "action\0setLumMappingMethod(QAction*)\0"
    "setColorCoord(QAction*)\0showAboutdialog()\0"
    "updateViewSize()\0"
};

const QMetaObject PFSViewMainWin::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PFSViewMainWin,
      qt_meta_data_PFSViewMainWin, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PFSViewMainWin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PFSViewMainWin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PFSViewMainWin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PFSViewMainWin))
        return static_cast<void*>(const_cast< PFSViewMainWin*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PFSViewMainWin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updatePointerValue(); break;
        case 1: updateRangeWindow(); break;
        case 2: updateChannelSelection(); break;
        case 3: setChannelSelection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: updateZoomValue(); break;
        case 5: gotoNextFrame(); break;
        case 6: gotoPreviousFrame(); break;
        case 7: saveImage(); break;
        case 8: copyImage(); break;
        case 9: setLumMappingMethod((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: setLumMappingMethod((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 11: setColorCoord((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 12: showAboutdialog(); break;
        case 13: updateViewSize(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
