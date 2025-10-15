# Serialization Reference

UnEngine does not use any serialization libraries, which means you have to manually define
Serialize and Deserialize functions for every data type you want to serialize.
There are predefined serialize and deserialize functions for many engine data types,
as well as a default function for fully sequential and trivially copyable data types. 

## Predefined serializers

To serialize and deserialize objects that already have these functions defined, 
simply call the template functions:
```c++
//The serialize function takes an object 
//It returns a byte array and it's length
std::pair<char*, size_t> data = une::Serialize(une::Vector3(0, 1, 2));
//The deserialize function takes the type of the object and a byte array
//It returns a new object and it's size
std::pair<une::Vector3, size_t> v = une::Deserialize<une::Vector3>(data);

//Make sure to delete the serialized data once you're done with it
delete[] data.first;
```

**IMPORTANT: The default function should not be used on data types that contain 
non sequential data or pointers, as the data in these will not be serialized!**
For example:
```c++
//This is serializable with the default
struct Foo
{
    int a;
    une::Vector3 vec3;
    const char name[4] = "Foo";
}
//This is NOT serializable with the default
struct Bar
{
    std::string str; //Not trivially copyable
    std::vector<int> v; //Not trivially copyable
    const char* name = "Bar"; //The data pointed to won't be serialized
}
```

## Custom serializers

To add serialize and deserialize functions for other data types using this system,
define a template specialization:

```c++
//Example for T = std::string
template<>
[[nodiscard]] inline std::pair<char*, size_t> une::Serialize<std::string>(std::string type)
{
    //Add a null terminator
    schar* buff = new char[type.size() + 1];
    memset(buff, 0, type.size() + 1);
    memcpy(buff, type.data(), type.size());
    return {buff, type.size() + 1};
}
template<>
std::pair<std::string, size_t> inline une::Deserialize<std::string>(const char* data)
{
    //Include the null terminator
    size_t size = strlen((char*)data) + 1;
    std::string result((char*)data, size);
    return {result, size};
}
```

## Packets

For sending serialized data over ENet you have to construct a packet with the binary data you want to send.
The Packet class doesn't necessarily have to be used with ENet and can be used as a binary stream

```c++
une::Packet packet;
//Any object with a defined une::Serialize function can be written to a packet with Write()
packet.Write(une::Vector3(1, 2, 3));
packet.Write(5.0);
packet.Write(5.f);
packet.Write(std::string("Hello World!"));
//Any object with a defined une::Deserialize function can be read from a packet with Read()
une::Vector3 v = packet.Read<une::Vector3>().first;
double d = packet.Read<double>().first;
float f = packet.Read<float>().first;
std::string s = packet.Read<std::string>().first;

//You can also directly read or write bytes
char bytes[4] = {0x45, 0x70, 0x69, 0x63};
packet.WriteBytes(bytes, 4);
std::vector<char> b = packet.ReadBytes(4);
```