using NodeGraphControl;
using NodeGraphControl.Editors;
using NodeGraphControl.Xml;
using NuiApiWrapper;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

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

        public ModuleNode(ModuleDescriptor descriptor, XmlTreeNode p_Input, NodeGraphView p_View) : base(p_Input, p_View)
        {
            Descriptor = descriptor;
        }

        public ModuleNode(ModuleDescriptor descriptor, int p_X, int p_Y, NodeGraphView p_View, bool p_CanBeSelected) : base(p_X, p_Y, p_View, p_CanBeSelected)
        {
            Descriptor = descriptor;
        }

        public override NodeGraphData Process()
        {
            return base.Process();
        }

        public override XmlTreeNode SerializeToXML(XmlTreeNode p_Parent)
        {
            var v_out = base.SerializeToXML(p_Parent);
            var descriptor = v_out.AddChild("ModuleDescriptor");

            return v_out;
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }
}
