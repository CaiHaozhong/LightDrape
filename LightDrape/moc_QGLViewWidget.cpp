/****************************************************************************
** Meta object code from reading C++ file 'QGLViewerWidget.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QGLViewerWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QGLViewerWidget.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGLViewerWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   17,   16,   16, 0x08,
      46,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QGLViewerWidget[] = {
    "QGLViewerWidget\0\0_mode\0slotDrawMode(QAction*)\0"
    "slotSnapshot()\0"
};

void QGLViewerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QGLViewerWidget *_t = static_cast<QGLViewerWidget *>(_o);
        switch (_id) {
        case 0: _t->slotDrawMode((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 1: _t->slotSnapshot(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QGLViewerWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QGLViewerWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_QGLViewerWidget,
      qt_meta_data_QGLViewerWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QGLViewerWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QGLViewerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QGLViewerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGLViewerWidget))
        return static_cast<void*>(const_cast< QGLViewerWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int QGLViewerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
