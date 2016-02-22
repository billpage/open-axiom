/****************************************************************************
** Meta object code from reading C++ file 'klfblockprocess.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "klfblockprocess.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'klfblockprocess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KLFBlockProcess[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      40,   22,   17,   16, 0x0a,
     103,   89,   17,   16, 0x2a,
     148,  140,   17,   16, 0x0a,
     190,  186,   17,   16, 0x2a,
     224,   16,  216,   16, 0x0a,
     243,   16,  216,   16, 0x0a,
     262,   16,   16,   16, 0x08,
     278,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KLFBlockProcess[] = {
    "KLFBlockProcess\0\0bool\0cmd,stdindata,env\0"
    "startProcess(QStringList,QByteArray,QStringList)\0"
    "cmd,stdindata\0startProcess(QStringList,QByteArray)\0"
    "cmd,env\0startProcess(QStringList,QStringList)\0"
    "cmd\0startProcess(QStringList)\0QString\0"
    "readStderrString()\0readStdoutString()\0"
    "ourProcExited()\0ourProcGotOurStdinData()\0"
};

void KLFBlockProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KLFBlockProcess *_t = static_cast<KLFBlockProcess *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->startProcess((*reinterpret_cast< QStringList(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])),(*reinterpret_cast< QStringList(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = _t->startProcess((*reinterpret_cast< QStringList(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->startProcess((*reinterpret_cast< QStringList(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->startProcess((*reinterpret_cast< QStringList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { QString _r = _t->readStderrString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: { QString _r = _t->readStdoutString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: _t->ourProcExited(); break;
        case 7: _t->ourProcGotOurStdinData(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KLFBlockProcess::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KLFBlockProcess::staticMetaObject = {
    { &QProcess::staticMetaObject, qt_meta_stringdata_KLFBlockProcess,
      qt_meta_data_KLFBlockProcess, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KLFBlockProcess::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KLFBlockProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KLFBlockProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KLFBlockProcess))
        return static_cast<void*>(const_cast< KLFBlockProcess*>(this));
    return QProcess::qt_metacast(_clname);
}

int KLFBlockProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProcess::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
