/****************************************************************************
** Meta object code from reading C++ file 'pfsview_widget.h'
**
** Created: Sat Mar 10 20:43:53 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pfsview_widget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pfsview_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PFSViewWidgetArea[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_PFSViewWidgetArea[] = {
    "PFSViewWidgetArea\0"
};

const QMetaObject PFSViewWidgetArea::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_PFSViewWidgetArea,
      qt_meta_data_PFSViewWidgetArea, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PFSViewWidgetArea::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PFSViewWidgetArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PFSViewWidgetArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PFSViewWidgetArea))
        return static_cast<void*>(const_cast< PFSViewWidgetArea*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int PFSViewWidgetArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_PFSViewWidget[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   14,   14,   14, 0x0a,
      45,   14,   14,   14, 0x0a,
      55,   14,   14,   14, 0x0a,
      77,   70,   14,   14, 0x0a,
     108,   70,   14,   14, 0x0a,
     137,   70,   14,   14, 0x0a,
     175,  168,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PFSViewWidget[] = {
    "PFSViewWidget\0\0updatePointerValue()\0"
    "zoomIn()\0zoomOut()\0zoomOriginal()\0"
    "action\0setRGBClippingMethod(QAction*)\0"
    "setInfNaNTreatment(QAction*)\0"
    "setNegativeTreatment(QAction*)\0method\0"
    "setLumMappingMethod(int)\0"
};

const QMetaObject PFSViewWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PFSViewWidget,
      qt_meta_data_PFSViewWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PFSViewWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PFSViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PFSViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PFSViewWidget))
        return static_cast<void*>(const_cast< PFSViewWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int PFSViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updatePointerValue(); break;
        case 1: zoomIn(); break;
        case 2: zoomOut(); break;
        case 3: zoomOriginal(); break;
        case 4: setRGBClippingMethod((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 5: setInfNaNTreatment((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 6: setNegativeTreatment((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 7: setLumMappingMethod((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void PFSViewWidget::updatePointerValue()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
