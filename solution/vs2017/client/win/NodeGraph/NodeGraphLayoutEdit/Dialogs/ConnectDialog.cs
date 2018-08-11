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
    public partial class ConnectDialog : Form
    {
        public bool IsOk = false;
        public string Endpoint;

        public ConnectDialog()
        {
            InitializeComponent();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            Endpoint = textBox1.Text;
            IsOk = true;
            Close();
        }

        public void SetDefaultUrl(string url)
        {
            textBox1.Text = url;
        }
    }
}
