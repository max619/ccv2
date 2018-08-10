using NodeGraphControl;
using NodeGraphControl.Editors;
using NuiApiWrapper;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Xml;

namespace NodeGraphControl
{
    public class ModuleNode : NodeGraphNode
    {
        
        [Browsable(false)]
        public ModuleDescriptor Descriptor { get; private set; }

        [Category("Properties")]
        public  List<nuiProperty> Properties => Descriptor.properties.ToList();

        public ModuleNode(ModuleDescriptor descriptor, NodeGraphView p_View) : base(descriptor, p_View)
        {
            Descriptor = descriptor;
        }

        public ModuleNode(XmlReader p_Input, NodeGraphView p_View) : base(p_Input, p_View)
        {
            InitNodeFromDescriptor(Descriptor, p_View);
        }

        public ModuleNode(ModuleDescriptor descriptor, int p_X, int p_Y, NodeGraphView p_View, bool p_CanBeSelected) : base(p_X, p_Y, p_View, p_CanBeSelected)
        {
            Descriptor = descriptor;
        }

        public override NodeGraphData Process()
        {
            return base.Process();
        }

        public override string ToString()
        {
            return base.ToString();
        }

        public override void WriteXml(XmlWriter writer)
        {
            base.WriteXml(writer);
            SerializeObject(Descriptor, writer);
        }

        public override void ReadXml(XmlReader reader)
        {
            base.ReadXml(reader);
            if(TryReadTill("ModuleDescriptor",reader))
                Descriptor = DeserializeObject<ModuleDescriptor>(reader);
        }
    }
}
