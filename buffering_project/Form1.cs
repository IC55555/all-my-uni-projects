using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace buffering_project
{

    public partial class Form1 : Form
    {
        LinkedList<string> file;
        CircularLinkedList<string> buffer;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {


        }


        private void button1_Click(object sender, EventArgs e)
        {
            file = new LinkedList<string>();
            buffer = new CircularLinkedList<string>();
            int BufferSize = Int32.Parse(textBox3.Text);
            int FileSize;
            if (Int32.Parse(textBox1.Text) % Int32.Parse(textBox2.Text) == 0)
                FileSize = Int32.Parse(textBox1.Text) / Int32.Parse(textBox2.Text);
            else
                FileSize = Int32.Parse(textBox1.Text) / Int32.Parse(textBox2.Text) + 1;
            
            for(int i = 0; i < FileSize; i++)
            {
                file.Add("text");
            }
            label6.Text = Convert.ToString(file.Count());
            label7.Text = Convert.ToString(buffer.Count());

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(file.getHead() == null)
            {
                MessageBox.Show(
                "Достигнут конец файла",
                "Сообщение",
                 MessageBoxButtons.OK,
                 MessageBoxIcon.Information,
                 MessageBoxDefaultButton.Button1,
                 MessageBoxOptions.DefaultDesktopOnly);
                return;
            }
            if(buffer.Count() == Int32.Parse(textBox3.Text))
            {
                MessageBox.Show(
               "Переполнение буфера",
               "Сообщение",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information,
                MessageBoxDefaultButton.Button1,
                MessageBoxOptions.DefaultDesktopOnly);
                return;
            }
            buffer.Add(file.getHead().Data);
            file.RemoveHead();
            label6.Text = Convert.ToString(file.Count());
            label7.Text = Convert.ToString(buffer.Count());

        }

        private void button3_Click(object sender, EventArgs e)
        {

            if (buffer.getHead() == null)
            {
                MessageBox.Show(
                "Буфер Пуст",
                "Сообщение",
                 MessageBoxButtons.OK,
                 MessageBoxIcon.Information,
                 MessageBoxDefaultButton.Button1,
                 MessageBoxOptions.DefaultDesktopOnly);
                return;
            }
            buffer.RemoveHead();
            //label6.Text = Convert.ToString(file.Count());
            label7.Text = Convert.ToString(buffer.Count());

        }


    }

    public class Node<T>
    {
        public Node(T data)
        {
            Data = data;
        }
        public T Data { get; set; }
        public Node<T> Next { get; set; }
    }

    public class LinkedList<T> : IEnumerable<T>  // односвязный список
    {
        Node<T> head; // головной/первый элемент
        Node<T> tail; // последний/хвостовой элемент
        int count;  // количество элементов в списке

        public Node<T> getHead()
        {
            return head;
        }
        // добавление элемента
        public void Add(T data)
        {
            Node<T> node = new Node<T>(data);

            if (head == null)
                head = node;
            else
                tail.Next = node;
            tail = node;

            count++;
        }

        public bool RemoveHead()
        {
            if (head == null)
                return false;
            else
            {
                head = head.Next;
                if (head == null)
                    tail = null;
                count--;
                return true;
            }
        }

        // удаление элемента
        public bool Remove(T data)
        {
            Node<T> current = head;
            Node<T> previous = null;

            while (current != null)
            {
                if (current.Data.Equals(data))
                {
                    // Если узел в середине или в конце
                    if (previous != null)
                    {
                        // убираем узел current, теперь previous ссылается не на current, а на current.Next
                        previous.Next = current.Next;

                        // Если current.Next не установлен, значит узел последний,
                        // изменяем переменную tail
                        if (current.Next == null)
                            tail = previous;
                    }
                    else
                    {
                        // если удаляется первый элемент
                        // переустанавливаем значение head
                        head = head.Next;

                        // если после удаления список пуст, сбрасываем tail
                        if (head == null)
                            tail = null;
                    }
                    count--;
                    return true;
                }

                previous = current;
                current = current.Next;
            }
            return false;
        }

        public int Count { get { return count; } }
        public bool IsEmpty { get { return count == 0; } }
        // очистка списка
        public void Clear()
        {
            head = null;
            tail = null;
            count = 0;
        }
        // содержит ли список элемент
        public bool Contains(T data)
        {
            Node<T> current = head;
            while (current != null)
            {
                if (current.Data.Equals(data))
                    return true;
                current = current.Next;
            }
            return false;
        }
        // добвление в начало
        public void AppendFirst(T data)
        {
            Node<T> node = new Node<T>(data);
            node.Next = head;
            head = node;
            if (count == 0)
                tail = head;
            count++;
        }
        // реализация интерфейса IEnumerable
        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable)this).GetEnumerator();
        }

        IEnumerator<T> IEnumerable<T>.GetEnumerator()
        {
            Node<T> current = head;
            while (current != null)
            {
                yield return current.Data;
                current = current.Next;
            }
        }
    }

    public class CircularLinkedList<T> : IEnumerable<T>  // кольцевой связный список
    {
        Node<T> head; // головной/первый элемент
        Node<T> tail; // последний/хвостовой элемент
        int count;  // количество элементов в списке

        public Node<T> getHead()
        {
            return head;
        }

        // добавление элемента
        public void Add(T data)
        {
            Node<T> node = new Node<T>(data);
            // если список пуст
            if (head == null)
            {
                head = node;
                tail = node;
                tail.Next = head;
            }
            else
            {
                node.Next = head;
                tail.Next = node;
                tail = node;
            }
            count++;
        }

        public bool RemoveHead()
        {
            if(head == tail)
            {
                head = null;
                tail = null;
                count = 0;
                return true;
            }
            if (head == null)
                return false;
            else
            {
                tail.Next = head.Next;
                head = head.Next;
                if (head == null)
                    tail = null;
                count--;
                return true;
            }
        }

        public bool Remove(T data)
        {
            Node<T> current = head;
            Node<T> previous = null;

            if (IsEmpty) return false;

            do
            {
                if (current.Data.Equals(data))
                {
                    // Если узел в середине или в конце
                    if (previous != null)
                    {
                        // убираем узел current, теперь previous ссылается не на current, а на current.Next
                        previous.Next = current.Next;

                        // Если узел последний,
                        // изменяем переменную tail
                        if (current == tail)
                            tail = previous;
                    }
                    else // если удаляется первый элемент
                    {

                        // если в списке всего один элемент
                        if (count == 1)
                        {
                            head = tail = null;
                        }
                        else
                        {
                            head = current.Next;
                            tail.Next = current.Next;
                        }
                    }
                    count--;
                    return true;
                }

                previous = current;
                current = current.Next;
            } while (current != head);

            return false;
        }

        public int Count { get { return count; } }
        public bool IsEmpty { get { return count == 0; } }

        public void Clear()
        {
            head = null;
            tail = null;
            count = 0;
        }

        public bool Contains(T data)
        {
            Node<T> current = head;
            if (current == null) return false;
            do
            {
                if (current.Data.Equals(data))
                    return true;
                current = current.Next;
            }
            while (current != head);
            return false;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable)this).GetEnumerator();
        }

        IEnumerator<T> IEnumerable<T>.GetEnumerator()
        {
            Node<T> current = head;
            do
            {
                if (current != null)
                {
                    yield return current.Data;
                    current = current.Next;
                }
            }
            while (current != head);
        }
    }

    
}
