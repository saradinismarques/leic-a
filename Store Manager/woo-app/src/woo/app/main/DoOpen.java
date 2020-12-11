package woo.app.main;

import java.io.FileNotFoundException;
import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.FileOpenFailedException;
import woo.exceptions.UnavailableFileException;
/**
 * Open existing saved state.
 */
public class DoOpen extends Command<Storefront> {

  private Input<String> _fileName;

  /** @param receiver */
  public DoOpen(Storefront receiver) {
    super(Label.OPEN, receiver);
    _fileName = _form.addStringInput(Message.openFile());
  }

  /** @see pt.tecnico.po.ui.Command#execute() */
  @Override
  public final void execute() throws DialogException {
    try{

      _form.parse();
      _receiver.load(_fileName.value());
      
    } catch (UnavailableFileException e){
      throw new FileOpenFailedException(_fileName.value());
    }  
  }
}
