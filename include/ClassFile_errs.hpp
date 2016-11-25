//
//  ClassFile_errs.h
//  
//
//  Created by Alfredo Cruz on 10/2/16.
//
//

#ifndef ClassFile_errs_h
#define ClassFile_errs_h

/***
 *
 *  Defines all the exceptions thrown in case of error
 *
 */

#pragma mark NoMagicNumber
/**********************************************************
 *           Thrown when no magic number match            *
 **********************************************************/
class NoMagicNumberMatchError{};

#pragma mark -

#pragma mark NoMemoryError
/**********************************************************
 *              Thrown when no memory error               *
 **********************************************************/
class NoMemoryError{};
#pragma mark -

#pragma mark TagNotRecognized
/**********************************************************
 *         Tag not recognized in the constant pool        *
 **********************************************************/
class ConstantPool_TagNotRecognized{};
#pragma mark -
/**********************************************************
 *         Invalid access flag for field                  *
 **********************************************************/
#pragma mark FieldInvalidAccessFlag
class FieldInvalidAccessFlag{};
#pragma mark -
/**********************************************************
 *         Final and volatile flags are set               *
 **********************************************************/
#pragma mark FieldInvalidFinalVolatileFlags
class FieldInvalidFinalVolatileFlags{};
#pragma mark -
/**********************************************************
 *    No ConstantValue is defined for static attribute    *
 **********************************************************/
#pragma mark NoConstantValueAttributeForStaticField
class NoConstantValueAttributeForStaticField{};
#pragma mark -
/**********************************************************
 *    We intend to push an attribute to a structure that  *
 *              shouldnt have any                         *
 **********************************************************/
#pragma mark FieldInfoDoesNotAdmitAttributes
class FieldInfoDoesNotAdmitAttributes{};
#pragma mark -
/**********************************************************
 *    New attribute index is greater than capacity        *
 **********************************************************/
#pragma mark FieldInfoNewAttributeIndexGraterThanCapacity
class FieldInfoNewAttributeIndexGraterThanCapacity{};
#pragma mark -
/**********************************************************
 *          Invalid index in the constant pool            *
 **********************************************************/
#pragma mark InvalidConstantPoolIndex
class InvalidConstantPoolIndex{};
#pragma mark -
/**********************************************************
 *      Unexpected type of data in the constant pool      *
 **********************************************************/
#pragma mark UnexpectedDataTypeAtConstantPoolIndex
class UnexpectedDataTypeAtConstantPoolIndex{};
#pragma mark -
/**********************************************************
 *          Invalid filename to dump bytecode             *
 **********************************************************/
#pragma mark InvalidFilenameToDumpBytecode
class InvalidFilenameToDumpBytecode{};
#pragma mark -
/**********************************************************
 *                  Invalid method index                  *
 **********************************************************/
#pragma mark InvalidFilenameToDumpBytecode
class InvalidMethodIndex{};
#pragma mark -
/**********************************************************
 *                Invalid instruction offset              *
 **********************************************************/
#pragma mark InvalidInstructionOffset
class InvalidInstructionOffset{};
#pragma mark -
/**********************************************************
 *                Invalid buffer size                     *
 **********************************************************/
#pragma mark InvalidOutputBufferSize
class InvalidOutputBufferSize{};
#pragma mark -

#endif /* ClassFile_errs_h */
