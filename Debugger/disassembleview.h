#ifndef DISASMMODEL_H
#define DISASMMODEL_H

#include <QObject>

#include <ppc/disassembler.h>
#include <util/memory.h>

class DisassembleView : public QObject
{
   Q_OBJECT

   Q_PROPERTY( quint32 startAddress READ startAddress WRITE setStartAddress )
   Q_PROPERTY( quint32 endAddress READ endAddress WRITE setEndAddress )
   Q_PROPERTY( quint32 rowCount READ rowCount NOTIFY onRowCountChanged )

public:
   DisassembleView() :
      m_endAddress(0), m_startAddress(0)
   {
   }

   ~DisassembleView()
   {
   }

   quint32 startAddress()
   {
      return m_startAddress;
   }

   quint32 endAddress()
   {
      return m_endAddress;
   }

   quint32 rowCount()
   {
      return (m_endAddress - m_startAddress) / 4;
   }

   void setStartAddress(quint32 address)
   {
      m_startAddress = address;
   }

   void setEndAddress(quint32 address)
   {
      m_endAddress = address;
      emit onRowCountChanged(rowCount());
   }

   Q_INVOKABLE QString address(quint32 index) const {
      return QString("%1").arg(m_startAddress + index * 4, 8, 16, QChar('0'));
   }

   Q_INVOKABLE QString code(quint32 index) const {
      ppc::Disassembler::State disState;
      ppc::Instruction ins;
      UINT32 address = index * 4 + m_startAddress;
      disState.cia = address;
      ins.value = Memory::read<uint32_t>(disState.cia);
      ppc::Disassembler::decode(&disState, ins);
      return QString::fromStdString("abc");
   }

   Q_INVOKABLE QString comment(quint32 index) const {
      return QString("abc123");
   }

signals:
   void onRowCountChanged(quint32 value);

private:
   quint32 m_startAddress;
   quint32 m_endAddress;
};

#endif
