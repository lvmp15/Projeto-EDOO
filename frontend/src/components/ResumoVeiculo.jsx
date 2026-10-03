import { infoTipo } from '../tipos.js'

const fundoIcone = { Carro: 'bg-sky-100', Moto: 'bg-amber-100', Caminhao: 'bg-green-100' }

// icone + placa + modelo + dono, usado no cartao da vaga e na janela de saida
export default function ResumoVeiculo({ veiculo, children }) {
  return (
    <>
      <div
        className={`grid size-12 shrink-0 place-items-center rounded-[10px] text-[28px] ${fundoIcone[veiculo.tipo] || 'bg-sky-100'}`}
      >
        {infoTipo(veiculo.tipo).icone}
      </div>

      <div className="flex min-w-0 flex-col gap-px text-[13px]">
        {/* placa no estilo Mercosul, com a faixa azul em cima */}
        <span className="self-start rounded border-[1.5px] border-t-5 border-slate-800 border-t-blue-700 px-1.5 font-mono text-sm font-bold tracking-widest text-slate-900">
          {veiculo.placa}
        </span>
        <strong className="text-sm">{veiculo.modelo}</strong>
        <small className="text-slate-500">{veiculo.dono}</small>
        {children}
      </div>
    </>
  )
}
