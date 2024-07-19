namespace SoundTunnelApp {
    partial class SoundTunnelForm {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            sourceComboBox = new ComboBox();
            sinkDeviceComboBox = new ComboBox();
            button1 = new Button();
            button2 = new Button();
            runningLabel = new Label();
            SuspendLayout();
            // 
            // sourceComboBox
            // 
            sourceComboBox.FormattingEnabled = true;
            sourceComboBox.Location = new Point(79, 40);
            sourceComboBox.Name = "sourceComboBox";
            sourceComboBox.Size = new Size(383, 23);
            sourceComboBox.TabIndex = 0;
            // 
            // sinkDeviceComboBox
            // 
            sinkDeviceComboBox.FormattingEnabled = true;
            sinkDeviceComboBox.Location = new Point(79, 97);
            sinkDeviceComboBox.Name = "sinkDeviceComboBox";
            sinkDeviceComboBox.Size = new Size(383, 23);
            sinkDeviceComboBox.TabIndex = 1;
            // 
            // button1
            // 
            button1.Location = new Point(79, 170);
            button1.Name = "button1";
            button1.Size = new Size(75, 23);
            button1.TabIndex = 2;
            button1.Text = "Start";
            button1.UseVisualStyleBackColor = true;
            button1.Click += OnStartButtonClicked;
            // 
            // button2
            // 
            button2.Location = new Point(79, 211);
            button2.Name = "button2";
            button2.Size = new Size(75, 23);
            button2.TabIndex = 3;
            button2.Text = "Stop";
            button2.UseVisualStyleBackColor = true;
            button2.Click += OnStopButtonClicked;
            // 
            // runningLabel
            // 
            runningLabel.AutoSize = true;
            runningLabel.Location = new Point(174, 174);
            runningLabel.Name = "runningLabel";
            runningLabel.Size = new Size(52, 15);
            runningLabel.TabIndex = 4;
            runningLabel.Text = "Running";
            runningLabel.Visible = false;
            // 
            // SoundTunnelForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(runningLabel);
            Controls.Add(button2);
            Controls.Add(button1);
            Controls.Add(sinkDeviceComboBox);
            Controls.Add(sourceComboBox);
            Name = "SoundTunnelForm";
            Text = "Sound Tunnel v2";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ComboBox sourceComboBox;
        private ComboBox sinkDeviceComboBox;
        private Button button1;
        private Button button2;
        private Label runningLabel;
    }
}
