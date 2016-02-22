/****************************************************************************
** Meta object code from reading C++ file 'klfdebug.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "klfdebug.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'klfdebug.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KLFDebugObjectWatcher[] = {

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
      23,   22,   22,   22, 0x0a,
      63,   56,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KLFDebugObjectWatcher[] = {
    "KLFDebugObjectWatcher\0\0"
    "debugObjectDestroyedFromSender()\0"
    "object\0debugObjectDestroyed(QObject*)\0"
};

void KLFDebugObjectWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KLFDebugObjectWatcher *_t = static_cast<KLFDebugObjectWatcher *>(_o);
        switch (_id) {
        case 0: _t->debugObjectDestroyedFromSender(); break;
        case 1: _t->debugObjectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KLFDebugObjectWatcher::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KLFDebugObjectWatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KLFDebugObjectWatcher,
      qt_meta_data_KLFDebugObjectWatcher, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KLFDebugObjectWatcher::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KLFDebugObjectWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KLFDebugObjectWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KLFDebugObjectWatcher))
        return static_cast<void*>(const_cast< KLFDebugObjectWatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int KLFDebugObjectWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
