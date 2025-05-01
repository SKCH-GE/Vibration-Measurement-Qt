#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDateTime>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

// // Define packet format
// #define PACKET_HEADER_1 0xAA
// #define PACKET_HEADER_2 0x55
// #define PACKET_SIZE 15  // 2 header + 12 data + 1 checksum
// #define MAX_HISTORY_SIZE 100


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_clearButton_clicked();
    void on_saveButton_clicked();
    void on_resetFilterButton_clicked();
    void on_serialPortComboBox_currentIndexChanged(int index);

    void readSerialData();
    void updatePlots();
    //void processPacket();

private:
    Ui::MainWindow *ui;

    // Serial communication
    QSerialPort *serialPort;
    QByteArray serialBuffer;
    QTimer *updateTimer;

    // Data structures
    struct SensorData {
        int16_t accelX, accelY, accelZ;
        int16_t gyroX, gyroY, gyroZ;
        QDateTime timestamp;
    };
    QVector<SensorData> dataHistory;
    static const int MAX_HISTORY_SIZE = 1000;

    // Packet details
    static const uint8_t PACKET_HEADER_1 = 0xAA;
    static const uint8_t PACKET_HEADER_2 = 0x55;
    static const int PACKET_SIZE = 15; // 2 header + 12 data + 1 checksum

    // Chart components
    QChartView *accelChartView;
    QChart *accelChart;
    QLineSeries *accelXSeries;
    QLineSeries *accelYSeries;
    QLineSeries *accelZSeries;
    QValueAxis *accelAxisX;
    QValueAxis *accelAxisY;

    QChartView *gyroChartView;
    QChart *gyroChart;
    QLineSeries *gyroXSeries;
    QLineSeries *gyroYSeries;
    QLineSeries *gyroZSeries;
    QValueAxis *gyroAxisX;
    QValueAxis *gyroAxisY;

    // Methods
    void setupCharts();
    void refreshSerialPorts();
    bool validateChecksum(const QByteArray &packet);
    void parsePacket(const QByteArray &packet);
    void saveDataToCSV(const QString &filename);
    void updateFilters();
};

#endif // MAINWINDOW_H
