using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;

namespace NodeGraphControl.Utils
{
    public abstract class XmlSerializibleBase : IXmlSerializable
    {
        const string _typeAttrName = "type";

        public XmlSchema GetSchema()
        {
            return null;
        }

        public abstract void ReadXml(XmlReader reader);
        public abstract void WriteXml(XmlWriter writer);

        protected void WriteType(XmlWriter writer, Type type)
        {
            writer.WriteAttributeString(_typeAttrName, type.FullName);
        }

        protected Type ReadType(XmlReader reader)
        {
            var typestr = reader.GetAttribute(_typeAttrName);
            return Type.GetType(typestr);
        }

        protected void SerializeObject<T>(T obj, XmlWriter writer)
        {
            var serializer = new XmlSerializer(typeof(T));
            serializer.Serialize(writer, obj);
        }

        protected T DeserializeObject<T>(XmlReader reader)
        {
            var serializer = new XmlSerializer(typeof(T));
            return (T)serializer.Deserialize(reader);
        }

        protected void ReadTill(string name, XmlReader reader)
        {
            while (reader.Name.IndexOf(name) == -1)
                reader.Read();
        }
        
        protected bool TryReadTill(string name, XmlReader reader)
        {
            try
            {
                if (reader.Name != name || reader.NodeType != XmlNodeType.Element)
                {
                    reader.Read();
                    while (reader.NodeType == XmlNodeType.Whitespace || reader.NodeType == XmlNodeType.EndElement)
                    {
                        reader.Read();
                    }
                    return reader.Name.IndexOf(name) != -1;
                }
                else return true;
            }
            finally
            {

            }
        }
    }
}
