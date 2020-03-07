// automatically generated by stateify.

package vfs2

import (
	"gvisor.dev/gvisor/pkg/state"
)

func (x *pollRestartBlock) beforeSave() {}
func (x *pollRestartBlock) save(m state.Map) {
	x.beforeSave()
	m.Save("pfdAddr", &x.pfdAddr)
	m.Save("nfds", &x.nfds)
	m.Save("timeout", &x.timeout)
}

func (x *pollRestartBlock) afterLoad() {}
func (x *pollRestartBlock) load(m state.Map) {
	m.Load("pfdAddr", &x.pfdAddr)
	m.Load("nfds", &x.nfds)
	m.Load("timeout", &x.timeout)
}

func init() {
	state.Register("pkg/sentry/syscalls/linux/vfs2.pollRestartBlock", (*pollRestartBlock)(nil), state.Fns{Save: (*pollRestartBlock).save, Load: (*pollRestartBlock).load})
}