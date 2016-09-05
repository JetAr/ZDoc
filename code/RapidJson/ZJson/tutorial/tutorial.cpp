/* #CC
       Zey_S : //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K.F1849258507----+----+----+----+
		//z
       Zey_E : //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K-+----+----+----+----+----+----+
       Zcl_S : //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K-+----+----+----+----+----+----+
       Zcl_E : //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K-+----+----+----+----+----+----+
     Zndex_S : //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K-+----+----+----+----+----+----+
     Zndex_E : //z 2016-08-19 15:49:25 L.134'29435 T1426790531.K ~75  +----+----+----+----+----+
\*    #HE //z 2016-08-19 15:25:03 L.134'30897 T1424274873.K************************************/

// Hello World example
// This example shows basic usage of DOM-style API.

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>

using namespace rapidjson;
using namespace std;

int main(int, char*[]) {
    ////////////////////////////////////////////////////////////////////////////
	// 1. Parse a JSON text string to a document.
	
	//z 构造 json 文档（mem 中的）；实际使用可能从文件
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    printf("Original JSON:\n %s\n", json);

	//z 使用默认模板参数，使用UTF8和 MemoryPoolAllocator 
    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

#if 0
    // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
    if (document.Parse(json).HasParseError())
        return 1;
#else
	//z 在 in-situ 解析的情况下，解码后的 strings 直接放在 source string 中。
	//z source 必须是str
    // In-situ parsing, decode strings directly in the source string. Source must be string.
    char buffer[sizeof(json)];
    memcpy(buffer, json, sizeof(json));
	//z 解析的结果放置在buffer中
    if (document.ParseInsitu(buffer).HasParseError())
        return 1;
#endif

    printf("\nParsing to document succeeded.\n");

    ////////////////////////////////////////////////////////////////////////////
    // 2. Access values in document. 
	
    printf("\nAccess values in document:\n");
	//z Document 是一个 json value 表示了dom的root，可以是一个obj或是array。
    assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

    assert(document.HasMember("hello"));
    assert(document["hello"].IsString());
	//z 看起来是 kv 对，只是前面是key，所得是value，根据调用不同函数，返回其值
    printf("hello = %s\n", document["hello"].GetString());

    // Since version 0.2, you can use single lookup to check the existing of member and its value:
	//z 从0.2版本开始，可以使用一个 lookup 来检查存在的member以及其值。
    Value::MemberIterator hello = document.FindMember("hello");
    assert(hello != document.MemberEnd());
    assert(hello->value.IsString());
    assert(strcmp("world", hello->value.GetString()) == 0);
	//z 这句是什么意思？
    (void)hello;

    assert(document["t"].IsBool());     // JSON true/false are bool. Can also uses more specific function IsTrue().
    printf("t = %s\n", document["t"].GetBool() ? "true" : "false");

    assert(document["f"].IsBool());
    printf("f = %s\n", document["f"].GetBool() ? "true" : "false");

    printf("n = %s\n", document["n"].IsNull() ? "null" : "?");

    assert(document["i"].IsNumber());   // Number is a JSON type, but C++ needs more specific type.
    assert(document["i"].IsInt());      // In this case, IsUint()/IsInt64()/IsUInt64() also return true.
    printf("i = %d\n", document["i"].GetInt()); // Alternative (int)document["i"]

    assert(document["pi"].IsNumber());
    assert(document["pi"].IsDouble());
    printf("pi = %g\n", document["pi"].GetDouble());

    {
		//z 使用一个 ref ，会更快
        const Value& a = document["a"]; // Using a reference for consecutive access is handy and faster.
        assert(a.IsArray());
		//z rapidjson 使用 SizeType 来替代 size_t 
        for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
            printf("a[%d] = %d\n", i, a[i].GetInt());
        
        int y = a[0].GetInt();
        (void)y;

        // Iterating array with iterators
        printf("a = ");
        for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
            printf("%d ", itr->GetInt());
        printf("\n");
    }

    // Iterating object members
    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
    for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
        printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);

    ////////////////////////////////////////////////////////////////////////////
    // 3. Modify values in document.

    // Change i to a bigger number
    {
        uint64_t f20 = 1;   // compute factorial of 20
        for (uint64_t j = 1; j <= 20; j++)
            f20 *= j;
        document["i"] = f20;    // Alternate form: document["i"].SetUint64(f20)
        assert(!document["i"].IsInt()); // No longer can be cast as int or uint.
    }

    // Adding values to array.
    {
        Value& a = document["a"];   // This time we uses non-const reference.
        Document::AllocatorType& allocator = document.GetAllocator();
        for (int i = 5; i <= 10; i++)
            a.PushBack(i, allocator);   // May look a bit strange, allocator is needed for potentially realloc. We normally uses the document's.

        // Fluent API
        a.PushBack("Lua", allocator).PushBack("Mio", allocator);
    }

    // Making string values.

    // This version of SetString() just store the pointer to the string.
    // So it is for literal and string that exists within value's life-cycle.
    {
        document["hello"] = "rapidjson";    // This will invoke strlen()
        // Faster version:
        // document["hello"].SetString("rapidjson", 9);
    }

    // This version of SetString() needs an allocator, which means it will allocate a new buffer and copy the the string into the buffer.
    Value author;
    {
        char buffer2[10];
        int len = sprintf(buffer2, "%s %s", "Milo", "Yip");  // synthetic example of dynamically created string.

        author.SetString(buffer2, static_cast<SizeType>(len), document.GetAllocator());
        // Shorter but slower version:
        // document["hello"].SetString(buffer, document.GetAllocator());

        // Constructor version: 
        // Value author(buffer, len, document.GetAllocator());
        // Value author(buffer, document.GetAllocator());
        memset(buffer2, 0, sizeof(buffer2)); // For demonstration purpose.
    }
    // Variable 'buffer' is unusable now but 'author' has already made a copy.
    document.AddMember("author", author, document.GetAllocator());

    assert(author.IsNull());        // Move semantic for assignment. After this variable is assigned as a member, the variable becomes null.

    ////////////////////////////////////////////////////////////////////////////
    // 4. Stringify JSON

    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());

    return 0;
}
