// classes do tailwind que se repetem em varios componentes
export const classeInput = 'rounded-lg border border-slate-300 bg-white px-2.5 py-2 font-normal'

export const classeRotulo = 'flex flex-col gap-1.5 text-[13px] font-semibold text-slate-600'

const variantes = {
  azul: 'bg-sky-600 px-3.5 py-2.5 text-white enabled:hover:bg-sky-700',
  cinza: 'bg-slate-200 px-3.5 py-2.5 text-slate-800 enabled:hover:bg-slate-300',
  saida: 'bg-red-100 px-3 py-2 text-red-700 enabled:hover:bg-red-200',
}

export function classeBotao(variante = 'azul') {
  return (
    'cursor-pointer rounded-lg font-semibold whitespace-nowrap transition-colors ' +
    'disabled:cursor-not-allowed disabled:opacity-45 ' +
    variantes[variante]
  )
}
