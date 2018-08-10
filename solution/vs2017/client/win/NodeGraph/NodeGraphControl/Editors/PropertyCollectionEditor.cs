using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Linq;
using System.Text;
using System.Windows.Forms.Design;

namespace NodeGraphControl.Editors
{
    public class PropertyCollectionEditor : UITypeEditor
    {
        //public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        //{
        //    if (context == null || context.Instance == null)
        //        return base.GetEditStyle(context);

        //    return UITypeEditorEditStyle.DropDown;
        //}

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            IWindowsFormsEditorService editorService;

            if (context == null || context.Instance == null || provider == null)
                return value;

            editorService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
                        
            return value;
            //return base.EditValue(context, provider, value);
        }
    }
}
