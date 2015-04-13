#include "chartwidget.h"
#include "ui_chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent): WorkerTab(parent), ui(new Ui::ChartWidget), _histogrammodel(nullptr), _databuffer(nullptr)
{
    this->_xycharticon = QIcon(":/misc_icons/res/xychart.png");
    this->_histogramicon = QIcon(":/misc_icons/res/histogram.png");

    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 1);
    ui->tbSwitchChart->setIcon(this->_xycharticon);

    connect(&this->_worker, SIGNAL(started()), this, SIGNAL(workStarted()));
    connect(&this->_worker, SIGNAL(occurrencesListCompleted()), this, SLOT(onOccurrencesListCompleted()));
    connect(&this->_worker, SIGNAL(dataEntropyCompleted()), this, SLOT(onDataEntropyCompleted()));
    connect(&this->_worker, SIGNAL(finished()), this, SIGNAL(workFinished()));
}

void ChartWidget::plot(IO::DataBuffer* databuffer)
{
    if(!databuffer || !databuffer->length())
        return;

    this->_databuffer = databuffer;
    this->_histogrammodel = new HistogramModel(this->_histogramchart, databuffer, ui->lisOccurrence);

    ui->lisOccurrence->setModel(this->_histogrammodel);
    this->updateEntropyText("Calculating...", qApp->palette().text().color());

    this->_worker.setData(&this->_histogramchart, &this->_entropychart, databuffer);
    this->_worker.start(QThread::LowPriority);
}

ChartWidget::~ChartWidget()
{
    if(this->_worker.isRunning())
    {
        this->_worker.abort();
        this->_worker.wait();
    }

    delete ui;
}

void ChartWidget::onOccurrencesListCompleted()
{
    this->_histogrammodel->updateStats();
    ui->chartcontainer->histogram()->setData(this->_histogramchart.result());

    for(int i = 0; i < this->_histogrammodel->columnCount(); i++)
        ui->lisOccurrence->resizeColumnToContents(i);

    ui->lisOccurrence->resizeRowsToContents();
    //FIXME: this->updateEntropy(occurrences);
}

void ChartWidget::onDataEntropyCompleted()
{
    /* FIXME:
    QList<QPointF> dataentropy = this->_worker.entropyChart();

    ui->chartcontainer->xyChart()->setXBase(16);
    ui->chartcontainer->xyChart()->setXRange(0, this->_databuffer->length());
    ui->chartcontainer->xyChart()->setYRange(0, 1);
    ui->chartcontainer->xyChart()->setPoints(dataentropy);
    */
}

void ChartWidget::updateEntropy(const QList<qint64> &occurrences)
{
    //FIXME: qreal e = entropy(occurrences, this->_hexeditdata->length());
    //FIXME: this->updateEntropyText(QString::number(e), ByteColors::entropyColor(e));
}

void ChartWidget::updateEntropyText(const QString &text, const QColor &forecolor)
{
    //FIXME: ui->lblEntropy->setText(text);

    //FIXME: QPalette p = ui->lblEntropy->palette();
    //FIXME: p.setColor(ui->lblEntropy->foregroundRole(), forecolor);
    //FIXME: ui->lblEntropy->setPalette(p);
}

void ChartWidget::on_tbHelp_clicked()
{
    ByteColorsDialog bcd(this);
    bcd.exec();
}

void ChartWidget::on_tbSwitchChart_clicked()
{
    ui->chartcontainer->switchChart();

    if(ui->chartcontainer->histogram()->isVisible())
        ui->tbSwitchChart->setIcon(this->_xycharticon);
    else
        ui->tbSwitchChart->setIcon(this->_histogramicon);
}
