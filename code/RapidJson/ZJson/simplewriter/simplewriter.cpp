/* #CC
       Zey_S : //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K.F1512763857----+----+----+----+
		//z
       Zey_E : //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K-+----+----+----+----+----+----+
       Zcl_S : //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K-+----+----+----+----+----+----+
       Zcl_E : //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K-+----+----+----+----+----+----+
     Zndex_S : //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K-+----+----+----+----+----+----+
     Zndex_E : //z 2016-08-19 16:45:26 L.134'26074 T2718114753.K ~3   +----+----+----+----+----+
\*    #HE //z 2016-08-19 16:42:58 L.134'26222 T2718007043.K************************************/

#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

using namespace rapidjson;
using namespace std;

int main() {
    StringBuffer s;
    //z Writer<StringBuffer> writer(s);
	PrettyWriter<StringBuffer> writer(s);
    writer.StartObject();               // Between StartObject()/EndObject(), 
    writer.Key("hello");                // output a key,
    writer.String("world");             // follow by a value.
    writer.Key("t");
    writer.Bool(true);
    writer.Key("f");
    writer.Bool(false);
    writer.Key("n");
    writer.Null();
    writer.Key("i");
    writer.Uint(123);
    writer.Key("pi");
    writer.Double(3.1416);
    writer.Key("a");
    writer.StartArray();                // Between StartArray()/EndArray(),
    for (unsigned i = 0; i < 4; i++)
        writer.Uint(i);                 // all values are elements of the array.
    writer.EndArray();
    writer.EndObject();

    // {"hello":"world","t":true,"f":false,"n":null,"i":123,"pi":3.1416,"a":[0,1,2,3]}
    cout << s.GetString() << endl;

    return 0;
}
