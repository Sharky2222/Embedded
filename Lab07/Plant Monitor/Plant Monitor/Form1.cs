using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Plant_Monitor
{
    public partial class Form1 : Form
    {
        int startflag = 0;
        int flag_sensor;
        string RxString;
        string temp = "30";

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
                serialPort1.Close();
            serialPort1.PortName = "COM9";
            serialPort1.BaudRate = 9600;
        }

        private void button1_Click(object sender, EventArgs e) //serial start
        {
            serialPort1.PortName = "COM9";
            serialPort1.BaudRate = 9600;
            serialPort1.Open();
            if (serialPort1.IsOpen)
            {
                //startSerial.Enabled = false;
                //serialStop.Enabled = true;
                textBox1.ReadOnly = false;
 
            }
 
        }

        private void SerialStop_Click(object sender, EventArgs e)
        {
            serialPort1.Close();
            //startSerial.Enabled = true;
            //serialStop.Enabled = false;
            textBox1.ReadOnly = true;
        }

        private void SerialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            RxString = serialPort1.ReadExisting();
            this.Invoke(new EventHandler(textBox1_TextChanged));
        }


        private void button3_Click(object sender, EventArgs e)
        {
            WebClient client = new WebClient();
            label1.Text = client.DownloadString("https://api.thingspeak.com/update?api_key=APFBZBJ3MZIIAM2V&field1=0");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            serialPort1.Close();
            //  startSerial.Enabled = true;
            //  serialStop.Enabled = false;
            textBox1.ReadOnly = true;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {
            
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
         
            textBox1.AppendText(RxString+"\n");
        
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (!string.Equals(textBox1.Text, ""))
            {
                if (serialPort1.IsOpen) serialPort1.Close();
                try
                {
                    /*  if (RxString[0] == 'B')
                      {
                          flag_sensor = 10;
                      }*/
                    const string WRITEKEY = "APFBZBJ3MZIIAM2V";
                    string strUpdateBase = "http://api.thingspeak.com/update";
                    string strUpdateURI = strUpdateBase + "?api_key=" + WRITEKEY;
                    string strField1 = textBox1.Text;
                    // string strField2 = "42";
                    HttpWebRequest ThingsSpeakReq;
                    HttpWebResponse ThingsSpeakResp;
                    strUpdateURI += "&field1=" + strField1;
                    /*if (flag_sensor == 11)
                    {
                        strUpdateURI += "&field1=" + strField1;
                    }
                    else if (flag_sensor == 12)
                    {
                        strUpdateURI += "&field2=" + strField1;
                    }
                    else if (flag_sensor == 13)
                    {
                        strUpdateURI += "&field3=" + strField1;
                    }
                    else if (flag_sensor == 14)
                    {
                        strUpdateURI += "&field4=" + strField1;
                    }
                    else
                    {
                    }*/
                    flag_sensor++;
                    ThingsSpeakReq = (HttpWebRequest)WebRequest.Create(strUpdateURI);
                    ThingsSpeakResp = (HttpWebResponse)ThingsSpeakReq.GetResponse();
                    ThingsSpeakResp.Close();
                    if (!(string.Equals(ThingsSpeakResp.StatusDescription, "OK")))
                    {
                        Exception exData = new Exception(ThingsSpeakResp.StatusDescription);
                        throw exData;
                    }
                }
                catch (Exception ex)
                {
                }
                textBox1.Text = "";
                serialPort1.Open();
            }
        }

    }
}
