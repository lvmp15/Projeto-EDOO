import { useEffect } from 'react'

export default function Aviso({ aviso, fechar }) {
  useEffect(() => {
    if (!aviso) return
    const tempo = setTimeout(fechar, aviso.tipo === 'erro' ? 6000 : 4000)
    return () => clearTimeout(tempo)
  }, [aviso])

  if (!aviso) return null

  const erro = aviso.tipo === 'erro'

  return (
    <div
      role={erro ? 'alert' : 'status'}
      className={`fixed right-5 bottom-5 z-20 flex max-w-[380px] items-start gap-2.5 rounded-[10px] border-l-5 bg-white px-3.5 py-3 text-sm shadow-xl transition duration-200 starting:translate-y-2.5 starting:opacity-0 ${erro ? 'border-red-600' : 'border-green-600'}`}
    >
      <span aria-hidden="true">{erro ? '⚠️' : '✅'}</span>
      <p className="flex-1">{aviso.texto}</p>
      <button onClick={fechar} aria-label="Fechar aviso" className="cursor-pointer text-lg leading-none text-slate-500">
        ×
      </button>
    </div>
  )
}
