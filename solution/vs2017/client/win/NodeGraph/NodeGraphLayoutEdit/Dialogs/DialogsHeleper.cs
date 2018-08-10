using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NodeGraphLayoutEdit.Dialogs
{
    public static class DialogsHeleper
    {
        public static bool ShowConnectDialog(out string endpoint)
        {
            var dialog = new ConnectDialog();
            dialog.ShowDialog();
            endpoint = dialog.Endpoint;
            return dialog.IsOk;
        }
    }
}
