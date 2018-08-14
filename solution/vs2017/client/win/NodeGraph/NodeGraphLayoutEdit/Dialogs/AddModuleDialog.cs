using NuiApiWrapper;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace NodeGraphLayoutEdit.Dialogs
{
    public partial class AddModuleDialog : Form
    {
        public bool isOk { get; private set; }
        List<ModuleDescriptor> Descriptors { get; set; }
        public ModuleDescriptor SelectedDescriptor { get; private set; }

        public AddModuleDialog(List<ModuleDescriptor> descriptors)
        {
            InitializeComponent();
            Descriptors = descriptors;

            InsertObjects();
        }

        private void InsertObjects()
        {
            foreach (var d in Descriptors)
            {
                ListViewItem item = new ListViewItem();
                item.Text = d.ToString(); // Or whatever display text you need
                item.Tag = d;
                listView1.Items.Add(item);
            }
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count > 0)
            {
                SelectedDescriptor = ((ListViewItem)listView1.SelectedItems[0]).Tag as ModuleDescriptor;
                propertyGrid2.SelectedObject = SelectedDescriptor;
            }
            else
            {
                SelectedDescriptor = null;
                propertyGrid2.SelectedObject = null;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            isOk = true;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            isOk = false;
            Close();
        }
    }
}
