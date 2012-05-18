/****************************************************************************
** Meta object code from reading C++ file 'luminancerange_widget.h'
**
** Created: Sat Mar 10 20:43:53 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "luminancerange_widget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'luminancerange_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LuminanceRangeWidget[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   21,   21,   21, 0x0a,
      61,   21,   21,   21, 0x0a,
      80,   21,   21,   21, 0x0a,
      94,   21,   21,   21, 0x0a,
     108,   21,   21,   21, 0x0a,
     128,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LuminanceRangeWidget[] = {
    "LuminanceRangeWidget\0\0updateRangeWindow()\0"
    "decreaseExposure()\0increaseExposure()\0"
    "extendRange()\0shrinkRange()\0"
    "fitToDynamicRange()\0lowDynamicRange()\0"
};

const QMetaObject LuminanceRangeWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LuminanceRangeWidget,
      qt_meta_data_LuminanceRangeWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LuminanceRangeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LuminanceRangeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LuminanceRangeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LuminanceRangeWidget))
        return static_cast<void*>(const_cast< LuminanceRangeWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int LuminanceRangeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateRangeWindow(); break;
        case 1: decreaseExposure(); break;
        case 2: increaseExposure(); break;
        case 3: extendRange(); break;
        case 4: shrinkRange(); break;
        case 5: fitToDynamicRange(); break;
        case 6: lowDynamicRange(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LuminanceRangeWidget::updateRangeWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
