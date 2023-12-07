using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace cube
{
    internal static class Program
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            Application.Run(new Form1());
           using (Form1 frm = new Form1())
            {
                // Показать форму перед созданием движка
                frm.Show();
                // Создать и инициализировать графический движок
                // нашей функцией
                frm.InitializeGraphics();
                // Войти в цикл сообщений
                Application.Run(frm);
            }
        }
    }
}
