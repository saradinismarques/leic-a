package woo.app.main;

import java.io.FileNotFoundException;
import java.io.IOException;
import woo.exceptions.MissingFileAssociationException;
import pt.tecnico.po.ui.Command;           
import pt.tecnico.po.ui.DialogException;                                    
import pt.tecnico.po.ui.Input;                                                      
import woo.Storefront;                                                                                                 
/**
 * Save current state to file under current name (if unnamed, query for name).
 */
public class DoSave extends Command<Storefront> {

  private Input<String> _fileName;

  /** @param receiver */
  public DoSave(Storefront receiver) {
    super(Label.SAVE, receiver);
    //FIXME init input fields
    if (_receiver.getFileName() == null)
      _fileName = _form.addStringInput(Message.newSaveAs());

  }

  /** @see pt.tecnico.po.ui.Command#execute() */
  @Override
  public final void execute() {
    try{

      _receiver.save();

    } catch(IOException e){
      e.printStackTrace();
    } catch(MissingFileAssociationException e){
      try{

        _fileName = _form.addStringInput(Message.newSaveAs());
        _form.parse();
        _receiver.saveAs(_fileName.value());
        
      } catch(IOException | MissingFileAssociationException f){
        f.printStackTrace();
      } 
    }
  }
}
