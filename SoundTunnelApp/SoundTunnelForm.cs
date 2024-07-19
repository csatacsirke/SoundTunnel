using Bch;

namespace SoundTunnelApp {
    public partial class SoundTunnelForm : Form {
        public SoundTunnelForm() {
            InitializeComponent();
        }



        class DeviceEntry {
            public BCAudioDevice Device = null!;
            public bool isDefault = false;

            public override string ToString() {
                string entryName = Device.GetName();
                return isDefault ?
                    "[DEFAULT] " + entryName :
                    entryName;
            }
        }

        List<DeviceEntry> m_sourceDevices = new List<DeviceEntry>();
        List<DeviceEntry> m_sinkDevices = new List<DeviceEntry>();

        BCSoundTunnel? m_soundTunnel;

        private void Form1_Load(object sender, EventArgs e) {

            this.Icon = Properties.Resources.hornicon;


            var defaultDevice = BCSoundTunnel.GetDefaultDevice();
            var devices = BCSoundTunnel.EnumerateDevices();


            m_sourceDevices.Clear();
            m_sinkDevices.Clear();

            m_sourceDevices.Add(new DeviceEntry {
                Device = defaultDevice,
                isDefault = true
            });

            foreach (var device in devices) {
                m_sourceDevices.Add(new DeviceEntry {
                    Device = device,
                });
                m_sinkDevices.Add(new DeviceEntry {
                    Device = device,
                });
            }

            sourceComboBox.DataSource = m_sourceDevices;
            sinkDeviceComboBox.DataSource = m_sinkDevices;

            foreach (DeviceEntry item in sinkDeviceComboBox.Items) {
                if (item.Device.GetName() != defaultDevice.GetName()) {
                    sinkDeviceComboBox.SelectedItem = item;
                    break;
                }
            }


        }

        private void OnStartButtonClicked(object sender, EventArgs e) {
            if (m_soundTunnel != null) {
                return;
            }

            var source = (sourceComboBox.SelectedItem as DeviceEntry)?.Device;
            var sink = (sinkDeviceComboBox.SelectedItem as DeviceEntry)?.Device;

            if (sink == null || source==null) {
                return;
            }

            
            m_soundTunnel = new BCSoundTunnel(source, sink);
            var m_soundTunnel_thread_local = m_soundTunnel;

            new Thread(() => {
                this.Invoke(() => {
                    this.runningLabel.Visible = true;
                });

                m_soundTunnel_thread_local.Run();

                this.Invoke(() => {
                    this.runningLabel.Visible = false;
                });
            }).Start();
        }

        private void OnStopButtonClicked(object sender, EventArgs e) {
            if(m_soundTunnel != null) {
                m_soundTunnel.Stop();
                m_soundTunnel = null;
            }
        }
    }
}
