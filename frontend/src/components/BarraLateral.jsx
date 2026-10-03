import { useState } from 'react'
import FormVeiculo from './FormVeiculo.jsx'
import FormCliente from './FormCliente.jsx'
import { infoTipo } from '../tipos.js'

export default function BarraLateral({ estado, executar }) {
  const [aba, setAba] = useState('veiculo')
  const [clienteNovo, setClienteNovo] = useState('')

  // volta pro cadastro de veiculo ja com o cliente novo escolhido como dono
  function clienteCadastrado(id) {
    setClienteNovo(String(id))
    setAba('veiculo')
  }

  return (
    <aside className="flex flex-col gap-4.5 border-b-6 border-sky-600 bg-white p-5 md:overflow-y-auto md:border-r-6 md:border-b-0">
      <div className="flex items-center gap-3">
        <div>
          <h1 className="text-lg font-bold">Sistema de Estacionamento</h1>
          <p className="text-[13px] text-slate-500">Projeto EDOO - Equipe 14</p>
        </div>
      </div>

      <div className="flex gap-1 rounded-[10px] bg-slate-100 p-1" role="tablist">
        <Aba ativa={aba === 'veiculo'} onClick={() => setAba('veiculo')}>
          Cadastrar veículo
        </Aba>
        <Aba ativa={aba === 'cliente'} onClick={() => setAba('cliente')}>
          Cadastrar cliente
        </Aba>
      </div>

      {aba === 'veiculo' ? (
        <FormVeiculo
          clientes={estado.clientes}
          executar={executar}
          clienteNovo={clienteNovo}
          irParaCliente={() => setAba('cliente')}
        />
      ) : (
        <FormCliente executar={executar} cadastrado={clienteCadastrado} />
      )}

      <ListaVeiculos veiculos={estado.veiculos} vagas={estado.vagas} />
    </aside>
  )
}

function Aba({ ativa, onClick, children }) {
  return (
    <button
      role="tab"
      aria-selected={ativa}
      onClick={onClick}
      className={`flex-1 cursor-pointer rounded-md px-1.5 py-2 text-sm font-semibold ${ativa ? 'bg-white text-slate-800 shadow-sm' : 'text-slate-500 hover:text-slate-700'}`}
    >
      {children}
    </button>
  )
}

function ListaVeiculos({ veiculos, vagas }) {
  function numeroDaVaga(vagaId) {
    const vaga = vagas.find((v) => v.id === vagaId)
    return vaga ? vaga.numero : '?'
  }

  return (
    <section>
      <h2 className="mb-2 flex items-center gap-2 text-[13px] font-bold tracking-wide text-slate-500 uppercase">
        Veículos cadastrados
        <span className="rounded-full bg-slate-100 px-2 text-xs">{veiculos.length}</span>
      </h2>

      {veiculos.length === 0 && <p className="text-[13px] text-slate-500">Nenhum veículo ainda.</p>}

      <ul className="flex flex-col gap-0.5">
        {veiculos.map((veiculo) => (
          <li key={veiculo.id} className="flex items-center gap-2.5 rounded-lg p-2 hover:bg-slate-50">
            <span className="text-xl">{infoTipo(veiculo.tipo).icone}</span>
            <div className="flex min-w-0 flex-1 flex-col">
              <strong>{veiculo.placa}</strong>
              <small className="truncate text-slate-500">
                {veiculo.modelo} · {veiculo.dono}
              </small>
            </div>
            {veiculo.vagaId ? (
              <span className="rounded-full bg-green-100 px-2 py-0.5 text-xs font-semibold whitespace-nowrap text-green-700">
                Vaga {numeroDaVaga(veiculo.vagaId)}
              </span>
            ) : (
              <span className="rounded-full bg-slate-100 px-2 py-0.5 text-xs font-semibold text-slate-500">Fora</span>
            )}
          </li>
        ))}
      </ul>
    </section>
  )
}
