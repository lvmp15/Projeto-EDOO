import { useEffect, useState } from 'react'
import ResumoVeiculo from './ResumoVeiculo.jsx'
import Seletor from './Seletor.jsx'
import { formatarDinheiro, METODOS } from '../tipos.js'
import { classeBotao, classeRotulo } from '../classes.js'

export default function ModalSaida({ vaga, veiculo, executar, fechar }) {
  const [metodo, setMetodo] = useState('')
  const [enviando, setEnviando] = useState(false)

  useEffect(() => {
    function teclaApertada(evento) {
      if (evento.key === 'Escape') fechar()
    }
    window.addEventListener('keydown', teclaApertada)
    return () => window.removeEventListener('keydown', teclaApertada)
  }, [fechar])

  async function confirmar() {
    setEnviando(true)
    const nomeMetodo = METODOS.find((m) => m.valor === metodo).nome
    const resposta = await executar(
      '/saida',
      { vagaId: vaga.id, metodo },
      (r) => `${r.placa} saiu da vaga ${vaga.numero} após ${r.permanencia}. Pago ${formatarDinheiro(r.valor)} no ${nomeMetodo}.`,
    )
    setEnviando(false)
    if (resposta) fechar()
  }

  return (
    <div className="fixed inset-0 z-10 grid place-items-center bg-slate-900/55 p-4" onClick={fechar}>
      <div
        role="dialog"
        aria-modal="true"
        aria-labelledby="titulo-saida"
        onClick={(e) => e.stopPropagation()}
        className="flex w-full max-w-[440px] flex-col gap-4 rounded-2xl bg-white p-5.5 shadow-2xl"
      >
        <h3 id="titulo-saida" className="text-lg font-bold">
          Registrar saída · Vaga {vaga.numero}
        </h3>

        <div className="flex items-center gap-3">
          <ResumoVeiculo veiculo={veiculo} />
        </div>

        <dl className="grid grid-cols-1 gap-2 sm:grid-cols-3">
          <Dado titulo="Entrada">{vaga.entrada}</Dado>
          <Dado titulo="Permanência">{vaga.permanencia}</Dado>
          <Dado titulo="Valor até agora">
            <span className="text-green-700">{formatarDinheiro(vaga.valorAtual)}</span>
          </Dado>
        </dl>

        <div className={classeRotulo}>
          <span>Forma de pagamento</span>
          <Seletor opcoes={METODOS} valor={metodo} escolher={setMetodo} />
        </div>

        <p className="text-[13px] text-slate-500">O valor final é calculado pelo C++ na hora da confirmação.</p>

        <div className="flex justify-end gap-2">
          <button className={classeBotao('cinza')} onClick={fechar}>
            Cancelar
          </button>
          <button className={classeBotao()} disabled={!metodo || enviando} onClick={confirmar}>
            Confirmar saída
          </button>
        </div>
      </div>
    </div>
  )
}

function Dado({ titulo, children }) {
  return (
    <div className="rounded-lg bg-slate-100 px-2.5 py-2">
      <dt className="text-xs text-slate-500">{titulo}</dt>
      <dd className="font-bold">{children}</dd>
    </div>
  )
}
