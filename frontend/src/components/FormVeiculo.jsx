import { useState } from 'react'
import Seletor from './Seletor.jsx'
import { TIPOS } from '../tipos.js'
import { classeBotao, classeInput, classeRotulo } from '../classes.js'

export default function FormVeiculo({ clientes, executar, clienteNovo, irParaCliente }) {
  const [placa, setPlaca] = useState('')
  const [modelo, setModelo] = useState('')
  const [tipo, setTipo] = useState('Carro')
  const [clienteId, setClienteId] = useState(clienteNovo)
  const [enviando, setEnviando] = useState(false)

  async function cadastrar(evento) {
    evento.preventDefault()
    setEnviando(true)
    const resposta = await executar('/veiculos', { placa, modelo, tipo, clienteId })
    setEnviando(false)

    // so limpa quando deu certo, assim da pra corrigir o campo errado sem digitar tudo de novo
    if (resposta) {
      setPlaca('')
      setModelo('')
    }
  }

  if (clientes.length === 0) {
    return (
      <div className="flex flex-col gap-2.5 rounded-[10px] bg-slate-100 p-3.5 text-sm">
        <p>Todo veículo precisa de um dono. Cadastre um cliente primeiro.</p>
        <button className={classeBotao()} onClick={irParaCliente}>
          Cadastrar cliente
        </button>
      </div>
    )
  }

  return (
    <form className="flex flex-col gap-3" onSubmit={cadastrar}>
      <label className={classeRotulo}>
        Placa
        <input
          className={classeInput}
          value={placa}
          onChange={(e) => setPlaca(e.target.value.toUpperCase())}
          placeholder="ABC1D23"
          maxLength={8}
          required
        />
      </label>

      <label className={classeRotulo}>
        Modelo
        <input
          className={classeInput}
          value={modelo}
          onChange={(e) => setModelo(e.target.value)}
          placeholder="Gol, Biz 125..."
          required
        />
      </label>

      <div className={classeRotulo}>
        <span>Tipo</span>
        <Seletor opcoes={TIPOS} valor={tipo} escolher={setTipo} />
      </div>

      <label className={classeRotulo}>
        Dono
        <select className={classeInput} value={clienteId} onChange={(e) => setClienteId(e.target.value)} required>
          <option value="">Escolha o cliente...</option>
          {clientes.map((cliente) => (
            <option key={cliente.id} value={cliente.id}>
              {cliente.nome}
            </option>
          ))}
        </select>
      </label>

      <button className={classeBotao()} disabled={enviando}>
        Cadastrar veículo
      </button>
    </form>
  )
}
