using NuiApiWrapper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NodeGraphLayoutEdit.Dialogs
{
    public static class DialogsHeleper
    {
        public static bool ShowConnectDialog(out string endpoint, string url = "tcp://127.0.0.1:7500")
        {
            var dialog = new ConnectDialog();
            dialog.SetDefaultUrl(url);
            dialog.ShowDialog();
            endpoint = dialog.Endpoint;
            return dialog.IsOk;
        }

        public static ModuleDescriptor AddModules(List<ModuleDescriptor> modules)
        {
            var dialog = new AddModuleDialog(modules);
            dialog.ShowDialog();
            if (dialog.isOk)
                return dialog.SelectedDescriptor;
            else
                return null;
        }
    }
}
