package woo.app.clients;

import woo.Storefront;
import pt.tecnico.po.ui.Command;    
import pt.tecnico.po.ui.Input;
import pt.tecnico.po.ui.DialogException;                                                                                           
import pt.tecnico.po.ui.Display;
import woo.app.exceptions.UnknownClientKeyException;
import woo.exceptions.UnknownClientKeyCoreException;
/**
 * Show client.
 */
public class DoShowClient extends Command<Storefront> {

  private Input<String> _key;

  public DoShowClient(Storefront storefront) {
    super(Label.SHOW_CLIENT, storefront);
    _key = _form.addStringInput(Message.requestClientKey());
  }

  @Override
  public void execute() throws DialogException {
    try {

      _form.parse();
      _display.popup(_receiver.getClient(_key.value()));
      
    } catch(UnknownClientKeyCoreException e){
      throw new UnknownClientKeyException(_key.value());
    }
  }
}
