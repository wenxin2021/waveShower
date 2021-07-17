#ifndef HEXTOSTRING_H
#define HEXTOSTRING_H

#include <QMainWindow>

class hexToString
{
public:
    hexToString();

    int strHexToDecimal(const QString &strHex);
    QString decimalToStrHex(int decimal);

    QString byteArrayToAsciiStr(const QByteArray &data);
    QByteArray hexStrToByteArray(const QString &str);
    char convertHexChar(char ch);
    QByteArray asciiStrToByteArray(const QString &str);
    QString byteArrayToHexStr(const QByteArray &data);

private:

};

#endif // HEXTOSTRING_H
