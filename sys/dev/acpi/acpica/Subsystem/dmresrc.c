/*******************************************************************************
 *
 * Module Name: dmresrc.c - Resource Descriptor disassembly
 *              xRevision: 1.29 $
 *
 ******************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2006, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/


#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: dmresrc.c,v 1.10 2006/01/29 03:05:47 kochi Exp $");

#include "acpi.h"
#include "amlcode.h"
#include "acdisasm.h"

#ifdef ACPI_DISASSEMBLER

#define _COMPONENT          ACPI_CA_DEBUGGER
        ACPI_MODULE_NAME    ("dbresrc")


/* Dispatch tables for Resource disassembly functions */

typedef
void (*ACPI_RESOURCE_HANDLER) (
    AML_RESOURCE            *Resource,
    UINT32                  Length,
    UINT32                  Level);

static ACPI_RESOURCE_HANDLER    AcpiGbl_DumpResourceDispatch [] =
{
    /* Small descriptors */

    NULL,                           /* 0x00, Reserved */
    NULL,                           /* 0x01, Reserved */
    NULL,                           /* 0x02, Reserved */
    NULL,                           /* 0x03, Reserved */
    AcpiDmIrqDescriptor,            /* 0x04, ACPI_RESOURCE_NAME_IRQ_FORMAT */
    AcpiDmDmaDescriptor,            /* 0x05, ACPI_RESOURCE_NAME_DMA_FORMAT */
    AcpiDmStartDependentDescriptor, /* 0x06, ACPI_RESOURCE_NAME_START_DEPENDENT */
    AcpiDmEndDependentDescriptor,   /* 0x07, ACPI_RESOURCE_NAME_END_DEPENDENT */
    AcpiDmIoDescriptor,             /* 0x08, ACPI_RESOURCE_NAME_IO_PORT */
    AcpiDmFixedIoDescriptor,        /* 0x09, ACPI_RESOURCE_NAME_FIXED_IO_PORT */
    NULL,                           /* 0x0A, Reserved */
    NULL,                           /* 0x0B, Reserved */
    NULL,                           /* 0x0C, Reserved */
    NULL,                           /* 0x0D, Reserved */
    AcpiDmVendorSmallDescriptor,    /* 0x0E, ACPI_RESOURCE_NAME_SMALL_VENDOR */
    NULL,                           /* 0x0F, ACPI_RESOURCE_NAME_END_TAG (not used) */

    /* Large descriptors */

    NULL,                           /* 0x00, Reserved */
    AcpiDmMemory24Descriptor,       /* 0x01, ACPI_RESOURCE_NAME_MEMORY_24 */
    AcpiDmGenericRegisterDescriptor,/* 0x02, ACPI_RESOURCE_NAME_GENERIC_REGISTER */
    NULL,                           /* 0x03, Reserved */
    AcpiDmVendorLargeDescriptor,    /* 0x04, ACPI_RESOURCE_NAME_LARGE_VENDOR */
    AcpiDmMemory32Descriptor,       /* 0x05, ACPI_RESOURCE_NAME_MEMORY_32 */
    AcpiDmFixedMemory32Descriptor,  /* 0x06, ACPI_RESOURCE_NAME_FIXED_MEMORY_32 */
    AcpiDmDwordDescriptor,          /* 0x07, ACPI_RESOURCE_NAME_DWORD_ADDRESS_SPACE */
    AcpiDmWordDescriptor,           /* 0x08, ACPI_RESOURCE_NAME_WORD_ADDRESS_SPACE */
    AcpiDmInterruptDescriptor,      /* 0x09, ACPI_RESOURCE_NAME_EXTENDED_XRUPT */
    AcpiDmQwordDescriptor,          /* 0x0A, ACPI_RESOURCE_NAME_QWORD_ADDRESS_SPACE */
    AcpiDmExtendedDescriptor        /* 0x0B, ACPI_RESOURCE_NAME_EXTENDED_ADDRESS_SPACE */
};


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmDumpInteger*
 *
 * PARAMETERS:  Value               - Value to emit
 *              Name                - Associated name (emitted as a comment)
 *
 * RETURN:      None
 *
 * DESCRIPTION: Integer output helper functions
 *
 ******************************************************************************/

void
AcpiDmDumpInteger8 (
    UINT8                   Value,
    const char              *Name)
{
    AcpiOsPrintf ("0x%2.2X,               // %s\n", Value, Name);
}

void
AcpiDmDumpInteger16 (
    UINT16                  Value,
    const char              *Name)
{
    AcpiOsPrintf ("0x%4.4X,             // %s\n", Value, Name);
}

void
AcpiDmDumpInteger32 (
    UINT32                  Value,
    const char              *Name)
{
    AcpiOsPrintf ("0x%8.8X,         // %s\n", Value, Name);
}

void
AcpiDmDumpInteger64 (
    UINT64                  Value,
    const char              *Name)
{
    AcpiOsPrintf ("0x%8.8X%8.8X, // %s\n",
        ACPI_FORMAT_UINT64 (ACPI_GET_ADDRESS (Value)), Name);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmBitList
 *
 * PARAMETERS:  Mask            - 16-bit value corresponding to 16 interrupt
 *                                or DMA values
 *
 * RETURN:      None
 *
 * DESCRIPTION: Dump a bit mask as a list of individual interrupt/DMA levels.
 *
 ******************************************************************************/

void
AcpiDmBitList (
    UINT16                  Mask)
{
    UINT32                  i;
    BOOLEAN                 Previous = FALSE;


    /* Open the initializer list */

    AcpiOsPrintf ("{");

    /* Examine each bit */

    for (i = 0; i < 16; i++)
    {
        /* Only interested in bits that are set to 1 */

        if (Mask & 1)
        {
            if (Previous)
            {
                AcpiOsPrintf (",");
            }
            Previous = TRUE;
            AcpiOsPrintf ("%d", i);
        }

        Mask >>= 1;
    }

    /* Close list */

    AcpiOsPrintf ("}\n");
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmResourceTemplate
 *
 * PARAMETERS:  Info            - Curent parse tree walk info
 *              ByteData        - Pointer to the byte list data
 *              ByteCount       - Length of the byte list
 *
 * RETURN:      None
 *
 * DESCRIPTION: Dump the contents of a Resource Template containing a set of
 *              Resource Descriptors.
 *
 ******************************************************************************/

void
AcpiDmResourceTemplate (
    ACPI_OP_WALK_INFO       *Info,
    UINT8                   *ByteData,
    UINT32                  ByteCount)
{
    ACPI_STATUS             Status;
    ACPI_NATIVE_UINT        CurrentByteOffset;
    UINT8                   ResourceType;
    UINT32                  ResourceLength;
    void                    *Aml;
    UINT32                  Level;
    BOOLEAN                 DependentFns = FALSE;
    UINT8                   ResourceIndex;


    Level = Info->Level;

    for (CurrentByteOffset = 0; CurrentByteOffset < ByteCount; )
    {
        Aml = &ByteData[CurrentByteOffset];

        /* Get the descriptor type and length */

        ResourceType = AcpiUtGetResourceType (Aml);
        ResourceLength = AcpiUtGetResourceLength (Aml);

        /* Validate the Resource Type and Resource Length */

        Status = AcpiUtValidateResource (Aml, &ResourceIndex);
        if (ACPI_FAILURE (Status))
        {
            AcpiOsPrintf ("/*** Could not validate Resource, type (%X) %s***/\n",
                ResourceType, AcpiFormatException (Status));
            return;
        }

        /* Point to next descriptor */

        CurrentByteOffset += AcpiUtGetDescriptorLength (Aml);

        /* Descriptor pre-processing */

        switch (ResourceType)
        {
        case ACPI_RESOURCE_NAME_START_DEPENDENT:

            /* Finish a previous StartDependentFns */

            if (DependentFns)
            {
                Level--;
                AcpiDmIndent (Level);
                AcpiOsPrintf ("}\n");
            }
            break;

        case ACPI_RESOURCE_NAME_END_DEPENDENT:

            Level--;
            DependentFns = FALSE;
            break;

        case ACPI_RESOURCE_NAME_END_TAG:

            /* Normal exit, the resource list is finished */

            if (DependentFns)
            {
                /*
                 * Close an open StartDependentDescriptor. This indicates a
                 * missing EndDependentDescriptor.
                 */
                Level--;
                DependentFns = FALSE;

                /* Go ahead and insert EndDependentFn() */

                AcpiDmEndDependentDescriptor (Aml, ResourceLength, Level);

                AcpiDmIndent (Level);
                AcpiOsPrintf (
                    "/*** Disassembler: inserted missing EndDependentFn () ***/\n");
            }
            return;

        default:
            break;
        }

        /* Disassemble the resource structure */

        AcpiGbl_DumpResourceDispatch [ResourceIndex] (
            Aml, ResourceLength, Level);

        /* Descriptor post-processing */

        if (ResourceType == ACPI_RESOURCE_NAME_START_DEPENDENT)
        {
            DependentFns = TRUE;
            Level++;
        }
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDmIsResourceTemplate
 *
 * PARAMETERS:  Op          - Buffer Op to be examined
 *
 * RETURN:      TRUE if this Buffer Op contains a valid resource
 *              descriptor.
 *
 * DESCRIPTION: Walk a byte list to determine if it consists of a valid set
 *              of resource descriptors.  Nothing is output.
 *
 ******************************************************************************/

BOOLEAN
AcpiDmIsResourceTemplate (
    ACPI_PARSE_OBJECT       *Op)
{
    ACPI_PARSE_OBJECT       *NextOp;
    UINT8                   *Aml;
    UINT8                   *EndAml;
    ACPI_SIZE               Length;


    /* This op must be a buffer */

    if (Op->Common.AmlOpcode != AML_BUFFER_OP)
    {
        return FALSE;
    }

    /* Get to the ByteData list */

    NextOp = Op->Common.Value.Arg;
    NextOp = NextOp->Common.Next;
    if (!NextOp)
    {
        return (FALSE);
    }

    /* Extract the data length and data pointer */

    Aml = NextOp->Named.Data;
    Length = (ACPI_SIZE) NextOp->Common.Value.Integer;

    /* Point to where the EndTag descriptor should be */

    EndAml = Aml + Length - sizeof (AML_RESOURCE_END_TAG);

    /*
     * The absolute minimum resource template is an EndTag (2 bytes),
     * and the list must be terminated by a valid 2-byte EndTag (length 1)
     */
    if ((Length < sizeof (AML_RESOURCE_END_TAG))  ||

        (*EndAml != (ACPI_RESOURCE_NAME_END_TAG | 1)))
    {
        return (FALSE);
    }

    /* Walk the byte list, abort on any invalid descriptor type or length */

    while (Aml <= EndAml)
    {
        /* Validate the Resource Type and Resource Length */

        if (ACPI_FAILURE (AcpiUtValidateResource (Aml, NULL)))
        {
            return (FALSE);
        }

        /* An EndTag descriptor terminates this resource template */

        if (AcpiUtGetResourceType (Aml) == ACPI_RESOURCE_NAME_END_TAG)
        {
            /*
             * For the resource template to be valid, one EndTag must appear
             * at the very end of the ByteList, not before
             */
            if (Aml != EndAml)
            {
                return (FALSE);
            }

            /*
             * All resource descriptors are valid, therefore this list appears
             * to be a valid resource template
             */
            return (TRUE);
        }

        /* This descriptor is valid, point to the next descriptor */

        Aml += AcpiUtGetDescriptorLength (Aml);
    }

    /* Did not find an EndTag, not a valid resource template */

    return (FALSE);
}

#endif
