// botoes lado a lado pra escolher uma opcao (tipo do veiculo e metodo de pagamento)
export default function Seletor({ opcoes, valor, escolher }) {
  return (
    <div className="grid grid-cols-3 gap-1.5">
      {opcoes.map((opcao) => {
        const ativa = valor === opcao.valor

        return (
          <button
            type="button"
            key={opcao.valor}
            aria-pressed={ativa}
            onClick={() => escolher(opcao.valor)}
            className={`flex cursor-pointer flex-col items-center gap-0.5 rounded-lg border px-1 py-2 text-[13px] font-semibold ${ativa ? 'border-sky-600 bg-sky-50 text-sky-700 ring-1 ring-sky-600 ring-inset' : 'border-slate-300 bg-white text-slate-500 hover:border-slate-400'}`}
          >
            <span className="text-xl">{opcao.icone}</span>
            {opcao.nome}
          </button>
        )
      })}
    </div>
  )
}
