package woo.app.clients;

import pt.tecnico.po.ui.Command;     
import pt.tecnico.po.ui.DialogException;                      
import pt.tecnico.po.ui.Input;                                 
import woo.Storefront;    
import woo.app.exceptions.DuplicateClientKeyException;
import woo.exceptions.DuplicateClientKeyCoreException;                                       
/**
 * Register new client.
 */
public class DoRegisterClient extends Command<Storefront> {

  private Input<String> _key, _name, _address;

  public DoRegisterClient(Storefront storefront) {
    super(Label.REGISTER_CLIENT, storefront);
    _key = _form.addStringInput(Message.requestClientKey());
    _name = _form.addStringInput(Message.requestClientName());
    _address = _form.addStringInput(Message.requestClientAddress());
  }

  @Override
  public void execute() throws DialogException {
    try {

      _form.parse();
      _receiver.registerClient(_key.value(), _name.value(), _address.value()); 
      
    } catch(DuplicateClientKeyCoreException e){
      throw new DuplicateClientKeyException(_key.value());
    }
  }
}
