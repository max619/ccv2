using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace NodeGraphControl.Utils
{
    public static class SerializationUtils
    {
        public static void Serialize<T>(Stream str, T obj)
        {
            var serializer = new XmlSerializer(typeof(T));
            serializer.Serialize(str, obj);
        }

        public static void Serialize<T>(string path, T obj)
        {
            using (var str = File.Create(path))
            {
                Serialize<T>(str, obj);
            }
        }

        public static T Deserialize<T>(Stream str)
        {
            var serializer = new XmlSerializer(typeof(T));
            return (T)serializer.Deserialize(str);
        }

        public static T Deserialize<T>(string path)
        {
            using (var str = File.OpenRead(path))
                return Deserialize<T>(str);
        }
    }
}
