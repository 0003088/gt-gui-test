package Elektra;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;

public interface Elektra extends Library {
	Elektra INSTANCE = (Elektra)
		Native.loadLibrary(("/usr/lib/libelektra.so"), Elektra.class);


/**************************************
 *
 * KDB methods
 *
 **************************************/

Pointer kdbOpen(Pointer p);
int kdbClose(Pointer handle, Pointer errorPointer);
int kdbGet(Pointer handle, Pointer returned,
	Pointer parentPointer);
int kdbSet(Pointer handle, Pointer returned,
	Pointer parentPointer);

/**************************************
 *
 * Key methods
 *
 **************************************/

Pointer keyNew(String name, Object... args);

Pointer keyDup(Pointer source);
int keyCopy(Pointer dest, Pointer source);

int keyClear(Pointer key);
int keyDel(Pointer key);

int keyIncRef(Pointer key);
int keyDecRef(Pointer key);
int keyGetRef(Pointer key);

/* Meta Info */
int keyRewindMeta(Pointer key);
Pointer keyNextMeta(Pointer key);
Pointer keyCurrentMeta(Pointer key);

int keyCopyMeta(Pointer dest, Pointer source, String metaName);
int keyCopyAllMeta(Pointer dest, Pointer source);

Pointer keyGetMeta(Pointer key, String  metaName);
int    keySetMeta(Pointer key, String  metaName,
	String newMetaString);


/* Methods for Making Tests */
int keyCmp(Pointer k1, Pointer k2);
int keyRel (Pointer k1, Pointer k2);

int keyNeedSync(Pointer key);

int keyIsBelow(Pointer key, Pointer check);
int keyIsBelowOrSame(Pointer key, Pointer check);
int keyIsDirectBelow(Pointer key, Pointer check);

int keyIsInactive(Pointer key);

int keyIsBinary(Pointer key);
int keyIsString(Pointer key);

/* Name Manipulation Methods */
String keyName(Pointer key);
int keyGetNameSize(Pointer key);
int keyGetName(Pointer key, String returnedName, int maxSize);

int keySetName(Pointer key, String newname);

int keyGetFullNameSize(Pointer key);
int keyGetFullName(Pointer key, String returnedName, int maxSize);

String keyBaseName(Pointer key);
int keyGetBaseNameSize(Pointer key);
int keyGetBaseName(Pointer key, String returned, int maxSize);

int keySetBaseName(Pointer key,String baseName);
int keyAddBaseName(Pointer key,String baseName);

/* Value Manipulation Methods */
// byte[] keyValue(Pointer key);
int keyGetValueSize(Pointer key);

String keyString(Pointer key);
int keyGetString(Pointer key, String returnedString, int maxSize);
int keySetString(Pointer key, String newString);

// int keyGetBinary(Pointer key, byte[] returnedBinary, int maxSize);
//int keySetBinary(Pointer key, byte[] newBinary, int dataSize);

Pointer ksNew(int alloc, Object... args);
Pointer ksLookup(Pointer ks, Pointer key, int options);
}