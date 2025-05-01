#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize serial port
    serialPort = new QSerialPort(this);
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);

    // Set up update timer
    updateTimer = new QTimer(this);
    updateTimer->setInterval(100); // 10 Hz update
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updatePlots);

    // Initialize UI components
    setupCharts();
    refreshSerialPorts();

    // Set initial UI state
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->clearButton->setEnabled(false);

    // Update filter parameters
    connect(ui->accelFilterSlider, &QSlider::valueChanged, this, &MainWindow::updateFilters);
    connect(ui->gyroFilterSlider, &QSlider::valueChanged, this, &MainWindow::updateFilters);

    // Set window title
    setWindowTitle("Vibration Monitoring System");
}

MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete ui;
}

void MainWindow::setupCharts()
{
    // Create accelerometer chart
    accelChart = new QChart();
    accelChart->setTitle("Accelerometer");
    accelChart->legend()->setAlignment(Qt::AlignBottom);

    accelXSeries = new QLineSeries();
    accelXSeries->setName("X-axis");
    accelYSeries = new QLineSeries();
    accelYSeries->setName("Y-axis");
    accelZSeries = new QLineSeries();
    accelZSeries->setName("Z-axis");

    accelChart->addSeries(accelXSeries);
    accelChart->addSeries(accelYSeries);
    accelChart->addSeries(accelZSeries);

    accelAxisX = new QValueAxis();
    accelAxisX->setTitleText("Samples");
    accelAxisX->setRange(0, 100);
    accelAxisY = new QValueAxis();
    accelAxisY->setTitleText("Acceleration (raw)");
    accelAxisY->setRange(-16384, 16384); // ±4g range with 16-bit resolution

    accelChart->setAxisX(accelAxisX, accelXSeries);
    accelChart->setAxisY(accelAxisY, accelXSeries);
    accelChart->setAxisX(accelAxisX, accelYSeries);
    accelChart->setAxisY(accelAxisY, accelYSeries);
    accelChart->setAxisX(accelAxisX, accelZSeries);
    accelChart->setAxisY(accelAxisY, accelZSeries);

    accelChartView = new QChartView(accelChart);
    accelChartView->setRenderHint(QPainter::Antialiasing);
    ui->accelChartLayout->addWidget(accelChartView);

    // Create gyroscope chart
    gyroChart = new QChart();
    gyroChart->setTitle("Gyroscope");
    gyroChart->legend()->setAlignment(Qt::AlignBottom);

    gyroXSeries = new QLineSeries();
    gyroXSeries->setName("X-axis");
    gyroYSeries = new QLineSeries();
    gyroYSeries->setName("Y-axis");
    gyroZSeries = new QLineSeries();
    gyroZSeries->setName("Z-axis");

    gyroChart->addSeries(gyroXSeries);
    gyroChart->addSeries(gyroYSeries);
    gyroChart->addSeries(gyroZSeries);

    gyroAxisX = new QValueAxis();
    gyroAxisX->setTitleText("Samples");
    gyroAxisX->setRange(0, 100);
    gyroAxisY = new QValueAxis();
    gyroAxisY->setTitleText("Angular Rate (raw)");
    gyroAxisY->setRange(-16384, 16384); // ±500°/s range with 16-bit resolution

    gyroChart->setAxisX(gyroAxisX, gyroXSeries);
    gyroChart->setAxisY(gyroAxisY, gyroXSeries);
    gyroChart->setAxisX(gyroAxisX, gyroYSeries);
    gyroChart->setAxisY(gyroAxisY, gyroYSeries);
    gyroChart->setAxisX(gyroAxisX, gyroZSeries);
    gyroChart->setAxisY(gyroAxisY, gyroZSeries);

    gyroChartView = new QChartView(gyroChart);
    gyroChartView->setRenderHint(QPainter::Antialiasing);
    ui->gyroChartLayout->addWidget(gyroChartView);
}

void MainWindow::refreshSerialPorts()
{
    ui->serialPortComboBox->clear();

    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPortInfos) {
        ui->serialPortComboBox->addItem(info.portName());
    }
}

void MainWindow::on_connectButton_clicked()
{
    if (!serialPort->isOpen()) {
        // Get selected serial port
        QString portName = ui->serialPortComboBox->currentText();
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "Warning", "No serial port selected.");
            return;
        }

        // Configure serial port
        serialPort->setPortName(portName);
        serialPort->setBaudRate(QSerialPort::Baud115200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        // Try to open the serial port
        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->connectButton->setText("Disconnect");
            ui->serialPortComboBox->setEnabled(false);
            ui->startButton->setEnabled(true);
            ui->statusBar->showMessage("Connected to " + portName);
        } else {
            QMessageBox::critical(this, "Error", "Could not open serial port: " + serialPort->errorString());
        }
    } else {
        // Disconnect
        serialPort->close();
        ui->connectButton->setText("Connect");
        ui->serialPortComboBox->setEnabled(true);
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        updateTimer->stop();
        ui->statusBar->showMessage("Disconnected");
    }
}

void MainWindow::on_startButton_clicked()
{
    // Start data acquisition
    updateTimer->start();
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->clearButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
    ui->statusBar->showMessage("Data acquisition started");
}

void MainWindow::on_stopButton_clicked()
{
    // Stop data acquisition
    updateTimer->stop();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->statusBar->showMessage("Data acquisition stopped");
}

void MainWindow::on_clearButton_clicked()
{
    // Clear data and charts
    dataHistory.clear();
    accelXSeries->clear();
    accelYSeries->clear();
    accelZSeries->clear();
    gyroXSeries->clear();
    gyroYSeries->clear();
    gyroZSeries->clear();
    ui->statusBar->showMessage("Data cleared");
}

void MainWindow::on_saveButton_clicked()
{
    // Get save file name
    QString filename = QFileDialog::getSaveFileName(this, "Save Data", "", "CSV Files (*.csv)");
    if (filename.isEmpty()) {
        return;
    }

    // Save data to CSV
    saveDataToCSV(filename);
}

void MainWindow::on_resetFilterButton_clicked()
{
    // Reset filter sliders to default values
    ui->accelFilterSlider->setValue(50); // 50% of max filter
    ui->gyroFilterSlider->setValue(50);
    updateFilters();
    ui->statusBar->showMessage("Filters reset to default");
}

void MainWindow::on_serialPortComboBox_currentIndexChanged(int index)
{
    // Enable or disable connect button based on port selection
    ui->connectButton->setEnabled(index >= 0);
}

void MainWindow::readSerialData()
{
    // Read all available data from the serial port
    QByteArray data = serialPort->readAll();
    serialBuffer.append(data);

    // Process complete packets
    while (serialBuffer.size() >= PACKET_SIZE) {
        // Look for packet header
        int headerPos = serialBuffer.indexOf(QByteArray(1, PACKET_HEADER_1));
        if (headerPos == -1) {
            // No header found, clear buffer
            serialBuffer.clear();
            break;
        }

        // Check if second header byte matches
        if (headerPos + 1 < serialBuffer.size() &&
            static_cast<uint8_t>(serialBuffer.at(headerPos + 1)) == PACKET_HEADER_2) {

            // Check if we have a complete packet
            if (headerPos + PACKET_SIZE <= serialBuffer.size()) {
                // Extract the packet
                QByteArray packet = serialBuffer.mid(headerPos, PACKET_SIZE);

                // Validate and parse the packet
                if (validateChecksum(packet)) {
                    parsePacket(packet);
                }

                // Remove processed data from buffer
                serialBuffer.remove(0, headerPos + PACKET_SIZE);
            } else {
                // Wait for more data
                break;
            }
        } else {
            // Remove up to the invalid header
            serialBuffer.remove(0, headerPos + 1);
        }
    }
}

bool MainWindow::validateChecksum(const QByteArray &packet)
{
    if (packet.size() != PACKET_SIZE) {
        return false;
    }

    // Calculate checksum (XOR of all bytes except the last one)
    uint8_t calculatedChecksum = 0;
    for (int i = 0; i < packet.size() - 1; i++) {
        calculatedChecksum ^= static_cast<uint8_t>(packet.at(i));
    }

    // Compare with received checksum
    uint8_t receivedChecksum = static_cast<uint8_t>(packet.at(packet.size() - 1));

    return calculatedChecksum == receivedChecksum;
}

void MainWindow::parsePacket(const QByteArray &packet)
{
    // Create a new sensor data entry
    SensorData data;
    data.timestamp = QDateTime::currentDateTime();

    // Parse accelerometer data
    data.accelX = static_cast<int16_t>((static_cast<uint8_t>(packet.at(2)) << 8) |
                                       static_cast<uint8_t>(packet.at(3)));
    data.accelY = static_cast<int16_t>((static_cast<uint8_t>(packet.at(4)) << 8) |
                                       static_cast<uint8_t>(packet.at(5)));
    data.accelZ = static_cast<int16_t>((static_cast<uint8_t>(packet.at(6)) << 8) |
                                       static_cast<uint8_t>(packet.at(7)));

    // Parse gyroscope data
    data.gyroX = static_cast<int16_t>((static_cast<uint8_t>(packet.at(8)) << 8) |
                                      static_cast<uint8_t>(packet.at(9)));
    data.gyroY = static_cast<int16_t>((static_cast<uint8_t>(packet.at(10)) << 8) |
                                      static_cast<uint8_t>(packet.at(11)));
    data.gyroZ = static_cast<int16_t>((static_cast<uint8_t>(packet.at(12)) << 8) |
                                      static_cast<uint8_t>(packet.at(13)));

    // Add to data history
    dataHistory.append(data);

    // Limit history size
    if (dataHistory.size() > MAX_HISTORY_SIZE) {
        dataHistory.removeFirst();
    }

    // Update status
    ui->statusBar->showMessage(QString("Received data: AccelX=%1, AccelY=%2, AccelZ=%3, GyroX=%4, GyroY=%5, GyroZ=%6")
                                   .arg(data.accelX).arg(data.accelY).arg(data.accelZ)
                                   .arg(data.gyroX).arg(data.gyroY).arg(data.gyroZ));
}

void MainWindow::updatePlots()
{
    // Clear existing data
    accelXSeries->clear();
    accelYSeries->clear();
    accelZSeries->clear();
    gyroXSeries->clear();
    gyroYSeries->clear();
    gyroZSeries->clear();

    // Set axis range based on history size
    int size = dataHistory.size();
    if (size > 0) {
        accelAxisX->setRange(0, size);
        gyroAxisX->setRange(0, size);

        // Plot the data
        for (int i = 0; i < size; i++) {
            const SensorData &data = dataHistory.at(i);

            // Accelerometer data
            accelXSeries->append(i, data.accelX);
            accelYSeries->append(i, data.accelY);
            accelZSeries->append(i, data.accelZ);

            // Gyroscope data
            gyroXSeries->append(i, data.gyroX);
            gyroYSeries->append(i, data.gyroY);
            gyroZSeries->append(i, data.gyroZ);
        }
    }
}

void MainWindow::saveDataToCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Write header
    out << "Timestamp,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ\n";

    // Write data
    for (const SensorData &data : dataHistory) {
        out << data.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << ","
            << data.accelX << ","
            << data.accelY << ","
            << data.accelZ << ","
            << data.gyroX << ","
            << data.gyroY << ","
            << data.gyroZ << "\n";
    }

    file.close();
    ui->statusBar->showMessage("Data saved to " + filename);
}

void MainWindow::updateFilters()
{
    // Send filter parameters to STM32
    if (serialPort->isOpen()) {
        // Format: [0xBB][accelFilter][gyroFilter]
        QByteArray command;
        command.append(static_cast<char>(0xBB));
        command.append(static_cast<char>(ui->accelFilterSlider->value()));
        command.append(static_cast<char>(ui->gyroFilterSlider->value()));

        serialPort->write(command);
        ui->statusBar->showMessage("Filter parameters updated");
    }
}
/*void MainWindow::processPacket()
{
    // Your implementation code here
    // For example:
    // Process the received data packet
    // Update UI elements
    // etc.
}*/
