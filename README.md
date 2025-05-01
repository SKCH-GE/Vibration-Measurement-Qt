
# Vibration Monitoring System (Qt/C++)

A desktop application developed with Qt to monitor and visualize real-time vibration data from an MPU6050 sensor connected via a serial interface. The app provides live accelerometer and gyroscope plots, allows data saving, and features basic filtering options.

---

## 🚀 Features

- **Real-time Serial Communication** at 115200 baud
- **Live Graphing** of:
  - Accelerometer data (X, Y, Z)
  - Gyroscope data (X, Y, Z)
- **Start/Stop/Save/Clear** buttons for data control
- **Low-pass Filter Sliders** to smooth data noise
- **CSV Export** for further analysis
- **Packet Validation** with simple checksum

---

## 🖼️ UI Overview

| Component       | Description                                    |
|----------------|------------------------------------------------|
| **Connect**     | Opens/closes the selected serial port          |
| **Start**       | Begins live data acquisition                   |
| **Stop**        | Pauses data capture                            |
| **Clear**       | Clears existing chart data                     |
| **Save**        | Saves all current data to a CSV file           |
| **Filter Sliders** | Smooth accelerometer/gyroscope readings   |
| **Charts**      | QtCharts-based real-time plots                 |

---

## 🛠 Architecture


![PP2x3i8m34NtV8L_eB1tG3p4m800bS9iDL6nb9SIb_Bvs4m8XdY8px6llAwCXQVW3GwS2lJL5QC5ccO5_G5Qs4xCAOBqASLgv4zRHwHufsZJB4X7OqJVL9yOWXiU6Aa6yuMeqRb2wC-fi1YdxEAoEBJBkH2xN_PofI0PMZYBb2mDC6MBxEelk7j8_2D673_u9ijS2oWlQnXny4_fRdV](https://github.com/user-attachments/assets/d9c8ea62-5a0d-47f8-b23e-583ff764c573)


**📦 Serial Packet Format**

Each data packet sent from the STM32 should follow this structure:

[0xAA][0xBB][accelX][accelY][accelZ][gyroX][gyroY][gyroZ][checksum]

    Header: 2 bytes (0xAA, 0xBB)

    Payload: 6 values × 2 bytes (signed 16-bit integers)

    Checksum: 1 byte, XOR of all bytes from header to last gyro value

⚙️ Build Instructions

Clone the repository:
```bash
git clone https://github.com/SKCH-GE/Vibration-Measurement-Qt.git
cd Vibration-Measurement-Qt
```

Open in Qt Creator or build manually:
```bash
qmake
make
```
Run the application:
```bash
./VibrationMonitor
```
Select the serial port, click Connect, and start streaming!

📚 Dependencies

    Qt 5.12+ (Tested with Qt 5.15)

    Qt Serial Port

    Qt Charts

You can install them via Qt Installer or your package manager.
📸 Screenshots

    Insert screenshots showing:

        Main application interface

        Real-time graph in action

        Filter sliders and Save/Connect buttons

🧠 Future Work

- Add FFT for frequency-domain vibration analysis
- Implement Kalman/IIR filter for advanced smoothing
- Add diagnostic tab with AI-based anomaly detection
	- Support data tagging for event labeling

📄 License

*This project is licensed under the MIT License.*
See the LICENSE file for more information.
🙌 Acknowledgements

- Built using the awesome Qt Framework
- Sensor: MPU6050 [Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf)
    


Let me know if you want to contribute to this project.
