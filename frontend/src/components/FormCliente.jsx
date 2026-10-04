import { useState } from 'react'
import { classeBotao, classeInput, classeRotulo } from '../classes.js'

export default function FormCliente({ executar, cadastrado }) {
  const [nome, setNome] = useState('')
  const [cpf, setCpf] = useState('')
  const [telefone, setTelefone] = useState('')
  const [enviando, setEnviando] = useState(false)

  async function cadastrar(evento) {
    evento.preventDefault()
    setEnviando(true)
    const resposta = await executar('/clientes', { nome, cpf, telefone })
    setEnviando(false)

    if (resposta) {
      cadastrado(resposta.id)
    }
  }

  return (
    <form className="flex flex-col gap-3" onSubmit={cadastrar}>
      <label className={classeRotulo}>
        Nome
        <input className={classeInput} value={nome} onChange={(e) => setNome(e.target.value)} placeholder="Ana Lima" required />
      </label>

      <label className={classeRotulo}>
        CPF
        <input
          className={classeInput}
          value={cpf}
          onChange={(e) => setCpf(e.target.value)}
          placeholder="000.000.000-00"
          required
        />
      </label>

      <label className={classeRotulo}>
        <span>
          Telefone <span className="font-normal text-slate-500">(opcional)</span>
        </span>
        <input
          className={classeInput}
          value={telefone}
          onChange={(e) => setTelefone(e.target.value)}
          placeholder="(81) 98888-7777"
        />
      </label>

      <button className={classeBotao()} disabled={enviando}>
        Cadastrar cliente
      </button>
    </form>
  )
}
