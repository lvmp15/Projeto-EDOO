import { useEffect, useState } from 'react'
import { buscarEstado, enviar } from './api.js'
import BarraLateral from './components/BarraLateral.jsx'
import Patio from './components/Patio.jsx'
import Aviso from './components/Aviso.jsx'

export default function App() {
  const [estado, setEstado] = useState({ clientes: [], veiculos: [], vagas: [] })
  const [carregando, setCarregando] = useState(true)
  const [erroConexao, setErroConexao] = useState('')
  const [aviso, setAviso] = useState(null)

  async function carregar() {
    try {
      setEstado(await buscarEstado())
      setErroConexao('')
    } catch (erro) {
      setErroConexao(erro.message)
    } finally {
      setCarregando(false)
    }
  }

  // recarrega a cada 30s pra permanencia e valor das vagas irem subindo
  useEffect(() => {
    carregar()
    const intervalo = setInterval(carregar, 30000)
    return () => clearInterval(intervalo)
  }, [])

  // todo botao que grava algo passa por aqui; devolve null se deu erro
  async function executar(rota, campos, montarMensagem) {
    try {
      const resposta = await enviar(rota, campos)
      setAviso({ tipo: 'ok', texto: montarMensagem ? montarMensagem(resposta) : resposta.mensagem })
      await carregar()
      return resposta
    } catch (erro) {
      setAviso({ tipo: 'erro', texto: erro.message })
      return null
    }
  }

  return (
    <div className="grid min-h-screen grid-cols-1 bg-slate-50 text-slate-800 md:h-screen md:grid-cols-[340px_1fr]">
      <BarraLateral estado={estado} executar={executar} />
      <Patio
        estado={estado}
        executar={executar}
        carregando={carregando}
        erroConexao={erroConexao}
        tentarDeNovo={carregar}
      />
      <Aviso aviso={aviso} fechar={() => setAviso(null)} />
    </div>
  )
}
