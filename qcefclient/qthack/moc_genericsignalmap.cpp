/****************************************************************************
** Meta object code from reading C++ file 'genericsignalmap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qthack/genericsignalmap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'genericsignalmap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GenericSignalMapper_t {
    QByteArrayData data[8];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GenericSignalMapper_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GenericSignalMapper_t qt_meta_stringdata_GenericSignalMapper = {
    {
QT_MOC_LITERAL(0, 0, 19), // "GenericSignalMapper"
QT_MOC_LITERAL(1, 20, 6), // "mapped"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 6), // "sender"
QT_MOC_LITERAL(4, 35, 11), // "QMetaMethod"
QT_MOC_LITERAL(5, 47, 6), // "signal"
QT_MOC_LITERAL(6, 54, 9), // "arguments"
QT_MOC_LITERAL(7, 64, 7) // "mapSlot"

    },
    "GenericSignalMapper\0mapped\0\0sender\0"
    "QMetaMethod\0signal\0arguments\0mapSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GenericSignalMapper[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QObjectStar, 0x80000000 | 4, QMetaType::QVariantList,    3,    5,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void GenericSignalMapper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GenericSignalMapper *_t = static_cast<GenericSignalMapper *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mapped((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< QMetaMethod(*)>(_a[2])),(*reinterpret_cast< QVariantList(*)>(_a[3]))); break;
        case 1: _t->mappingSignal(_a); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GenericSignalMapper::*_t)(QObject * , QMetaMethod , QVariantList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GenericSignalMapper::mapped)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject GenericSignalMapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GenericSignalMapper.data,
      qt_meta_data_GenericSignalMapper,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GenericSignalMapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GenericSignalMapper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GenericSignalMapper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GenericSignalMapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GenericSignalMapper::mapped(QObject * _t1, QMetaMethod _t2, QVariantList _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
