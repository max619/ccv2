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

    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class nuiProperty
    {
        [XmlAttribute]
        [Category("Info"), DisplayName("Name")]
        public string name { get; set; }

        [XmlAttribute]
        [Browsable(false)]
        public int type { get; set; }

        [ReadOnly(true)]
        public string description { get; set; }

        [XmlAttribute]
        [Category("Value"), DisplayName("Value")]
        public string value { get; set; }

        [Category("Info"), DisplayName("Type")]
        [EditorBrowsable(EditorBrowsableState.Never)]
        public nuiPropertyType ReadableType { get => ((nuiPropertyType)type); set => type = (int)value; }

        public override string ToString()
        {
            return name;
        }
    }
}
