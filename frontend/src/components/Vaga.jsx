import { useState } from 'react'
import ResumoVeiculo from './ResumoVeiculo.jsx'
import { infoTipo, formatarDinheiro } from '../tipos.js'
import { classeBotao, classeInput } from '../classes.js'

export default function Vaga({ vaga, lado, veiculos, executar, abrirSaida }) {
  const [escolhido, setEscolhido] = useState('')
  const tipo = infoTipo(vaga.tipo)
  const veiculo = veiculos.find((v) => v.id === vaga.veiculoId)
  const direita = lado === 'direita'

  // o select so mostra veiculos do mesmo tipo da vaga que estao fora do estacionamento
  const disponiveis = veiculos.filter((v) => v.tipo === vaga.tipo && !v.vagaId)

  async function alocar() {
    const resposta = await executar('/entrada', { vagaId: vaga.id, veiculoId: escolhido })
    if (resposta) setEscolhido('')
  }

  function remover() {
    if (window.confirm(`Remover a vaga ${vaga.numero}?`)) {
      executar('/vagas/remover', { vagaId: vaga.id })
    }
  }

  // -mb-1 sobrepoe a linha de baixo com a de cima da proxima vaga, ficando uma linha amarela so
  return (
    <div
      className={`group relative -mb-1 flex min-h-30 items-center border-y-4 border-yellow-400 px-4 py-3.5 md:px-5 ${direita ? 'md:col-start-3 md:justify-end' : 'md:col-start-1'}`}
    >
      <span
        aria-hidden="true"
        className={`pointer-events-none absolute top-1/2 right-5 -translate-y-1/2 text-6xl font-extrabold text-white/10 ${direita ? 'md:right-auto md:left-5' : ''}`}
      >
        {vaga.numero}
      </span>

      <div className={`relative flex max-w-full flex-col items-start gap-2 ${direita ? 'md:items-end' : ''}`}>
        <span className="text-xs font-semibold tracking-wide text-white/60 uppercase">
          Vaga {vaga.numero} · {tipo.icone} {tipo.nome}
        </span>

        {veiculo ? (
          // starting: e o estado de quando o cartao aparece, ai ele desliza vindo da pista do meio
          <div
            className={`flex max-w-full items-center gap-3 rounded-xl bg-white px-3 py-2.5 shadow-lg shadow-black/30 transition duration-500 ease-out starting:translate-x-16 starting:opacity-0 motion-reduce:transition-none ${direita ? 'md:starting:-translate-x-16' : ''}`}
          >
            <ResumoVeiculo veiculo={veiculo}>
              <small className="text-slate-500">
                Entrou {vaga.entrada} · {vaga.permanencia} · {formatarDinheiro(vaga.valorAtual)}
              </small>
            </ResumoVeiculo>
            <button className={`${classeBotao('saida')} ml-1`} onClick={abrirSaida}>
              Saída
            </button>
          </div>
        ) : vaga.ocupada ? (
          <p className="text-sm text-red-300">Ocupada, mas sem ticket aberto.</p>
        ) : (
          <div className="flex max-w-full gap-2">
            <select
              className={`${classeInput} w-[230px] min-w-0 disabled:border-transparent disabled:bg-white/10 disabled:text-white/55`}
              value={escolhido}
              onChange={(e) => setEscolhido(e.target.value)}
              disabled={disponiveis.length === 0}
              aria-label={`Veículo para a vaga ${vaga.numero}`}
            >
              <option value="">
                {disponiveis.length > 0 ? 'Escolha o veículo...' : `Sem ${tipo.nome.toLowerCase()} disponível`}
              </option>
              {disponiveis.map((v) => (
                <option key={v.id} value={v.id}>
                  {v.placa} · {v.modelo}
                </option>
              ))}
            </select>
            <button className={classeBotao()} disabled={!escolhido} onClick={alocar}>
              Alocar
            </button>
          </div>
        )}
      </div>

      {!vaga.ocupada && !veiculo && (
        <button
          onClick={remover}
          title="Remover vaga"
          aria-label={`Remover vaga ${vaga.numero}`}
          className={`absolute top-2 right-2.5 grid size-6.5 cursor-pointer place-items-center rounded-md bg-white/10 text-lg leading-none text-white/70 opacity-0 transition-opacity group-hover:opacity-100 hover:bg-red-600 hover:text-white focus-visible:opacity-100 pointer-coarse:opacity-100 ${direita ? 'md:right-auto md:left-2.5' : ''}`}
        >
          ×
        </button>
      )}
    </div>
  )
}
