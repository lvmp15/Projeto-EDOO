import { useState } from 'react'
import Vaga from './Vaga.jsx'
import ModalSaida from './ModalSaida.jsx'
import { TIPOS } from '../tipos.js'
import { classeBotao, classeInput } from '../classes.js'

export default function Patio({ estado, executar, carregando, erroConexao, tentarDeNovo }) {
  const { vagas, veiculos } = estado
  const [numero, setNumero] = useState('')
  const [tipoNovaVaga, setTipoNovaVaga] = useState('Carro')
  const [idSaindo, setIdSaindo] = useState(null)

  const proximoNumero = vagas.reduce((maior, vaga) => Math.max(maior, vaga.numero), 0) + 1
  const ocupadas = vagas.filter((vaga) => vaga.veiculoId).length

  // busca de novo a cada render pra janela de saida mostrar o valor atualizado
  const vagaSaindo = vagas.find((vaga) => vaga.id === idSaindo && vaga.veiculoId)
  const veiculoSaindo = vagaSaindo && veiculos.find((v) => v.id === vagaSaindo.veiculoId)

  async function adicionarVaga(evento) {
    evento.preventDefault()
    const resposta = await executar('/vagas', { numero: numero || proximoNumero, tipo: tipoNovaVaga })
    if (resposta) setNumero('')
  }

  function desenharPatio() {
    if (erroConexao) {
      return (
        <div className="mx-auto my-15 flex max-w-[440px] flex-col items-center gap-3.5 px-4 text-center text-white/80">
          <p>{erroConexao}</p>
          <button className={classeBotao()} onClick={tentarDeNovo}>
            Tentar de novo
          </button>
        </div>
      )
    }

    if (carregando) {
      return <p className="my-15 text-center text-white/80">Carregando...</p>
    }

    if (vagas.length === 0) {
      return <p className="my-15 px-4 text-center text-white/80">Nenhuma vaga ainda. Use "Adicionar vaga" aqui em cima.</p>
    }

    // vagas alternam de lado (1 esquerda, 2 direita...) e a faixa da pista ocupa todas as linhas do meio
    return (
      <div className="mx-auto grid max-w-[1200px] grid-cols-1 md:grid-cols-[minmax(0,1fr)_96px_minmax(0,1fr)]">
        <div
          aria-hidden="true"
          className="mx-auto hidden w-0 border-l-4 border-dashed border-white/50 md:col-start-2 md:block"
          style={{ gridRow: `1 / span ${Math.ceil(vagas.length / 2)}` }}
        />
        {vagas.map((vaga, i) => (
          <Vaga
            key={vaga.id}
            vaga={vaga}
            lado={i % 2 === 0 ? 'esquerda' : 'direita'}
            veiculos={veiculos}
            executar={executar}
            abrirSaida={() => setIdSaindo(vaga.id)}
          />
        ))}
      </div>
    )
  }

  return (
    <main className="flex min-w-0 flex-col md:overflow-hidden">
      <header className="flex flex-wrap items-center justify-between gap-x-6 gap-y-3 border-b border-slate-200 bg-white px-4 py-3.5 md:px-6">
        <div>
          <h2 className="text-xl font-bold">Pátio</h2>
          <p className="mt-0.5 text-[13px] text-slate-500">
            {vagas.length - ocupadas} livres · {ocupadas} ocupadas · {vagas.length} no total
          </p>
        </div>

        <form className="flex w-full flex-wrap items-center gap-2 sm:w-auto sm:flex-nowrap" onSubmit={adicionarVaga}>
          <label className="flex items-center gap-1.5 text-sm font-semibold text-slate-500">
            Nº
            <input
              type="number"
              min="1"
              className={`${classeInput} w-19`}
              value={numero}
              placeholder={String(proximoNumero)}
              onChange={(e) => setNumero(e.target.value)}
            />
          </label>
          <select
            className={`${classeInput} flex-1 sm:flex-none`}
            value={tipoNovaVaga}
            onChange={(e) => setTipoNovaVaga(e.target.value)}
            aria-label="Tipo da vaga"
          >
            {TIPOS.map((t) => (
              <option key={t.valor} value={t.valor}>
                {t.icone} {t.nome}
              </option>
            ))}
          </select>
          <button className={`${classeBotao()} w-full sm:w-auto`}>+ Adicionar vaga</button>
        </form>
      </header>

      <div className="flex-1 bg-[#33383f] py-8 md:overflow-y-auto">{desenharPatio()}</div>

      {vagaSaindo && veiculoSaindo && (
        <ModalSaida vaga={vagaSaindo} veiculo={veiculoSaindo} executar={executar} fechar={() => setIdSaindo(null)} />
      )}
    </main>
  )
}
