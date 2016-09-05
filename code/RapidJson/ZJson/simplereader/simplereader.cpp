/* #CC
       Zey_S : //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K.F592137725 ----+----+----+----+
		//z
       Zey_E : //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K-+----+----+----+----+----+----+
       Zcl_S : //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K-+----+----+----+----+----+----+
       Zcl_E : //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K-+----+----+----+----+----+----+
     Zndex_S : //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K-+----+----+----+----+----+----+
     Zndex_E : //z 2016-08-19 16:40:38 L.134'26362 T2717935103.K ~18  +----+----+----+----+----+
\*    #HE //z 2016-08-19 16:20:54 L.134'27546 T2715563323.K************************************/

#include "rapidjson/reader.h"
#include <iostream>

using namespace rapidjson;
using namespace std;

struct MyHandler {
    bool Null() { cout << "Null()" << endl; return true; }
    bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }
    bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
    bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
    bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
    bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
    bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
    bool RawNumber(const char* str, SizeType length, bool copy) { 
        cout << "Number(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool String(const char* str, SizeType length, bool copy) { 
        cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool StartObject() { cout << "StartObject()" << endl; return true; }
    bool Key(const char* str, SizeType length, bool copy) {
        cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
    bool StartArray() { cout << "StartArray()" << endl; return true; }
    bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
};

int main() {
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

    MyHandler handler;
    Reader reader;
    StringStream ss(json);

    reader.Parse(ss, handler);

    return 0;
}

/*Output
StartObject()
Key(hello, 5, true)
String(world, 5, true)
Key(t, 1, true)
Bool(true)
Key(f, 1, true)
Bool(false)
Key(n, 1, true)
Null()
Key(i, 1, true)
Uint(123)
Key(pi, 2, true)
Double(3.1416)
Key(a, 1, true)
StartArray()
Uint(1)
Uint(2)
Uint(3)
Uint(4)
EndArray(4)
EndObject(7)
*/
