using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace NuiApiWrapper
{
    public enum nuiPropertyType
    {
        NUI_PROPERTY_NONE,
        NUI_PROPERTY_BOOL,
        NUI_PROPERTY_STRING,
        NUI_PROPERTY_INTEGER,
        NUI_PROPERTY_DOUBLE,
        NUI_PROPERTY_FLOAT,
        NUI_PROPERTY_POINTLIST,
    }

    public class nuiProperty
    {
        [XmlAttribute]
        [Category("Info"), DisplayName("Name")]
        [ReadOnly(true)]
        public string name { get; set; }

        [XmlAttribute]
        [Browsable(false)]
        public int type { get; set; }

        [XmlAttribute]
        [Category("Value"), DisplayName("Value")]
        public string value { get; set; }

        [Category("Info"), DisplayName("Type")]
        [EditorBrowsable(EditorBrowsableState.Never)]
        public string ReadableType => ((nuiPropertyType)type).ToString();

        public override string ToString()
        {
            return name;
        }
    }
}
