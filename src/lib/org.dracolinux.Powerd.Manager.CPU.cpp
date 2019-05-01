#include "org.dracolinux.Powerd.Manager.CPU.h"
#include <QFile>
#include <QTextStream>

int PowerCpu::getTotal()
{
    int counter = 0;
    while(1) {
        if (QFile::exists(QString("%1/cpu%2")
                          .arg(LINUX_CPU_SYS)
                          .arg(counter))) { counter++; }
        else { break; }
    }
    if (counter>0) { return counter; }
    return -1;
}

const QString PowerCpu::getGovernor(int cpu)
{
    QString result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = gov.readAll().trimmed();
        gov.close();
    }
    return result;
}

const QStringList PowerCpu::getGovernors()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getGovernor(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList PowerCpu::getAvailableGovernors()
{
    QStringList result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNORS));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
    return result;
}

bool PowerCpu::governorExists(const QString &gov)
{
    if (gov.isEmpty()) { return false; }
    return getAvailableGovernors().contains(gov);
}

bool PowerCpu::setGovernor(const QString &gov, int cpu)
{
    if (!governorExists(gov)) { return false; }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << gov;
        file.close();
        if (gov == getGovernor(cpu)) { return true;}
    }
    return false;
}

bool PowerCpu::setGovernor(const QString &gov)
{
    if (!governorExists(gov)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setGovernor(gov, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

const QString PowerCpu::getFrequency(int cpu)
{
    QString result;
    QFile freq(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCY));
    if (!freq.exists()) { return result; }
    if (freq.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = freq.readAll().trimmed();
        freq.close();
    }
    return result;
}

const QStringList PowerCpu::getFrequencies()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getFrequency(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList PowerCpu::getAvailableFrequency()
{
    QStringList result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCIES));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
    return result;
}

bool PowerCpu::frequencyExists(const QString &freq)
{
    if (freq.isEmpty()) { return false; }
    return getAvailableFrequency().contains(freq);
}

bool PowerCpu::setFrequency(const QString &freq, int cpu)
{
    if (!frequencyExists(freq)) { return false; }
    if (getGovernor(cpu) != "userspace") {
        if (!setGovernor("userspace", cpu)) { return false; }
    }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_SET_SPEED));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << freq;
        file.close();
        if (freq == getFrequency(cpu)) { return true;}
    }
    return false;
}

bool PowerCpu::setFrequency(const QString &freq)
{
    if (!frequencyExists(freq)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setFrequency(freq, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}
