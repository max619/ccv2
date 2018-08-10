using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace NuiApiWrapper
{
    public class EndpointDescriptor
    {
        [XmlAttribute]
        public int index;
        [XmlAttribute]
        public string descriptor;
    }
}
